"""Rebuild and check the per-release columns of src/ff8/kernel_item_layouts.h.

The English 2013 column is the reference: which native function each row is in (reached from
FFNx externals through calls / absolute references) and where its operand sits. For every other
release this script follows the same references in that executable, finds the same operands, and
checks them against the release's own arrays. It also scans each release for references to the
moved arrays the table does not list.

    py -3 -m pip install capstone pefile
    py -3 tests/ff8_items/generate_layouts.py C:/FF8Releases            # check
    py -3 tests/ff8_items/generate_layouts.py C:/FF8Releases --write    # rewrite the columns

The folder holds FF8_EN.exe, FF8_FR.exe, FF8_DE.exe, FF8_ES.exe, FF8_IT.exe, FF8_JP.exe,
FF8_JP_NV.exe (2013 releases) and FF8_EN2000.exe (the 1.2 US FF8.exe). Read only.
"""
import collections
import pathlib
import re
import sys

import capstone
import pefile

ROOT = pathlib.Path(__file__).resolve().parents[2]
HEADER = ROOT / "src/ff8/kernel_item_layouts.h"
TEST_EXTERNALS = ROOT / "tests/ff8_items/release_externals.h"
FF8_DATA = ROOT / "src/ff8_data.cpp"

# (file, FFNx version name, layout index)
RELEASES = [("EN", "US_NV", 0), ("FR", "FR_NV", 1), ("DE", "DE_NV", 2), ("ES", "SP_NV", 3),
            ("IT", "IT_NV", 4), ("JP", "JP", 5), ("JP_NV", "JP_NV", 5), ("EN2000", "US", 0)]
LAYOUTS = 6
VERSIONS = {"US": 5, "US_NV": 6, "FR": 7, "FR_NV": 8, "DE": 9, "DE_NV": 10, "SP": 11, "SP_NV": 12,
            "IT": 13, "IT_NV": 14, "US_EIDOS": 15, "US_EIDOS_NV": 16, "JP": 17, "JP_NV": 18}
STARTS = {"US": 0x55AC07, "US_NV": 0x55ADB7, "FR_NV": 0x55B2E7, "DE_NV": 0x55B3E7, "SP_NV": 0x55B3D7,
          "IT_NV": 0x55B367, "JP": 0x55F487, "JP_NV": 0x55F6E7}

# English functions the table leaves alone on purpose although they use moved arrays:
# start, size, why. Their counterparts are found by signature in each release.
LEFT_ALONE = [(0x47F320, 0x1C3E, "battle debug menu"), (0x482A20, 0x160, "debug give-all-items"),
              (0x4EBBA0, 0x29A, "ShopMenu_Enter, never called")]

md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True


class Image:
    def __init__(self, path):
        pe = pefile.PE(str(path), fast_load=True)
        self.base = pe.OPTIONAL_HEADER.ImageBase
        self.mem = bytearray(pe.OPTIONAL_HEADER.SizeOfImage)
        for s in pe.sections:
            data = s.get_data()
            self.mem[s.VirtualAddress : s.VirtualAddress + len(data)] = data
            if s.Name.rstrip(b"\0") == b".text":
                self.text = (self.base + s.VirtualAddress, self.base + s.VirtualAddress + s.Misc_VirtualSize)

    def read(self, ea, n):
        return bytes(self.mem[ea - self.base : ea - self.base + n])

    def u32(self, ea):
        return int.from_bytes(self.read(ea, 4), "little")

    def insns(self, start, size):
        return list(md.disasm(self.read(start, size), start))


# --- FFNx externals, as ff8_find_externals() computes them ---------------------------------
def evaluate_externals(img, vname):
    src = re.sub(r"//[^\n]*", "", re.sub(r"/\*.*?\*/", "", FF8_DATA.read_text(encoding="utf8"), flags=re.S))
    i = src.index("void ff8_find_externals()")
    j = src.index("{", i)
    depth, k = 0, j
    while True:
        depth += {"{": 1, "}": -1}.get(src[k], 0)
        if depth == 0:
            break
        k += 1
    body = src[j + 1 : k]
    version = VERSIONS[vname]
    vars_ = {"ff8_externals.start": STARTS[vname]}

    def cond(c):
        e = c.replace("JP_VERSION", str(version in (17, 18))).replace("FF8_US_VERSION", str(version in (5, 6, 15, 16)))
        e = re.sub(r"VERSION_FF8_12_(\w+)", lambda m: str(VERSIONS[m.group(1)]), e)
        e = re.sub(r"\bversion\b", str(version), e).replace("&&", " and ").replace("||", " or ")
        return bool(eval(re.sub(r"!(?!=)", " not ", e), {"True": True, "False": False}))

    def split_top(e, chars):
        depth, out, last = 0, [], 0
        for n, ch in enumerate(e):
            depth += 1 if ch in "([" else -1 if ch in ")]" else 0
            if depth == 0 and ch in chars and n > 0 and e[n - 1] not in "*/(eExX":
                out.append((e[last:n], ch))
                last = n + 1
        return out + [(e[last:], "")]

    def value(e):
        e = e.strip()
        while True:
            m = re.match(r"^\((?:const\s+)?[A-Za-z_][\w:\s]*(?:\s*\*+)?(?:\s*\(\s*\*\s*\)\s*\([^()]*\))?\s*\)\s*(?=[A-Za-z_(])", e)
            if not m:
                break
            e = e[m.end() :].strip()
        e = re.sub(r"^(?:uint32_t|DWORD|int|uint8_t|char\s*\*)\s*\((.*)\)$", r"\1", e, flags=re.S)
        depth = 0
        for n, ch in enumerate(e):
            depth += 1 if ch == "(" else -1 if ch == ")" else 0
            if ch == "?" and depth == 0:
                rest = e[n + 1 :]
                d2 = 0
                for q, c2 in enumerate(rest):
                    d2 += 1 if c2 == "(" else -1 if c2 == ")" else 0
                    if c2 == ":" and d2 == 0 and rest[q : q + 2] != "::":
                        return value(rest[:q]) if cond(e[:n]) else value(rest[q + 1 :])
        parts = split_top(e, "+-")
        if len(parts) > 1:
            total, op = 0, "+"
            for text, nxt in parts:
                total = total + value(text) if op == "+" else total - value(text)
                op = nxt
            return total & 0xFFFFFFFF
        if e.startswith("(") and e.endswith(")"):
            return value(e[1:-1])
        m = re.match(r"^get_relative_call\((.*),(.*)\)$", e, re.S)
        if m:
            a = value(m.group(1)) + value(m.group(2))
            return (a + 5 + int.from_bytes(img.read(a + 1, 4), "little", signed=True)) & 0xFFFFFFFF
        m = re.match(r"^get_absolute_value\((.*),(.*)\)$", e, re.S)
        if m:
            return img.u32(value(m.group(1)) + value(m.group(2)))
        m = re.match(r"^\*\s*\(\s*\(\s*int32_t\s*\*\s*\)\s*\((.*)\)\s*\)$", e, re.S)
        if m:
            return int.from_bytes(img.read(value(m.group(1)), 4), "little", signed=True) & 0xFFFFFFFF
        m = re.match(r"^((?:ff8|common)_externals\.\w+)\[(.+)\](\.func)?$", e)
        if m:
            return img.u32(vars_[m.group(1)] + value(m.group(2)) * (8 if m.group(3) else 4))
        if re.match(r"^0[xX][0-9A-Fa-f]+$|^\d+$", e):
            return int(e, 0)
        if re.match(r"^(?:ff8|common)_externals\.\w+$", e):
            return vars_[e]
        raise ValueError(e)

    def run(code):
        pos = 0
        while pos < len(code):
            while pos < len(code) and code[pos].isspace():
                pos += 1
            if pos >= len(code):
                return
            if re.match(r"if\b", code[pos:]):
                p0 = code.index("(", pos)
                depth, q = 0, p0
                while True:
                    depth += {"(": 1, ")": -1}.get(code[q], 0)
                    if depth == 0:
                        break
                    q += 1
                c = code[p0 + 1 : q]
                then, pos = block(code, q + 1)
                els = ""
                m = re.match(r"\s*else\b", code[pos:])
                if m:
                    els, pos = block(code, pos + m.end())
                try:
                    run(then if cond(c) else els)
                except Exception:  # a condition on something not modelled here
                    pass
                continue
            if code[pos] == "{":
                inner, pos = block(code, pos)
                run(inner)
                continue
            end = code.find(";", pos)
            if end < 0:
                return
            m = re.match(r"^((?:ff8|common)_externals\.\w+)\s*=\s*(.+)$", code[pos:end].strip(), re.S)
            if m:
                try:
                    vars_[m.group(1)] = value(m.group(2))
                except Exception:  # constructs this evaluator does not model (spans, structs)
                    pass
            pos = end + 1

    def block(code, pos):
        while code[pos].isspace():
            pos += 1
        if code[pos] != "{":
            if re.match(r"if\b", code[pos:]):
                # a nested if as the single statement: take it up to its end
                start = pos
                depth = 0
                while True:
                    if code[pos] == "{":
                        depth += 1
                    elif code[pos] == "}":
                        depth -= 1
                    elif code[pos] == ";" and depth == 0:
                        rest = code[pos + 1 :]
                        if not re.match(r"\s*else\b", rest):
                            return code[start : pos + 1], pos + 1
                    pos += 1
            end = code.index(";", pos)
            return code[pos : end + 1], end + 1
        depth, start = 0, pos
        while True:
            depth += {"{": 1, "}": -1}.get(code[pos], 0)
            pos += 1
            if depth == 0:
                return code[start + 1 : pos - 1], pos

    run(body)
    return vars_


# --- the header ------------------------------------------------------------------------------
def parse_offsets(text):
    text = text.strip()
    m = re.match(r"same\((0x[0-9A-F]+)\)", text)
    if m:
        return [int(m.group(1), 16)] * LAYOUTS
    return [int(x, 16) for x in re.findall(r"0x[0-9A-F]+", text)]


def parse_header():
    h = HEADER.read_text(encoding="utf8")
    fns = re.findall(r"^\t(fn_\w+),$", h[h.index("enum item_fn") :], re.M)
    roots = dict(re.findall(r"^\tfn\[(fn_\w+)\] = (.+);$", h, re.M))
    refs = [(a, b, c == "true", parse_offsets(o)) for a, b, c, o in
            re.findall(r"^\t\{ (fn_\w+), (fn_\w+), (true|false), (same\(0x[0-9A-F]+\)|\{[^}]*\}) \},$", h, re.M)]
    sites = [dict(fn=a, offsets=parse_offsets(o), width=int(w), target=t, native=int(n, 16), value=int(v, 16)) for a, o, w, t, n, v in
             re.findall(r"^\t\{ (fn_\w+), (same\(0x[0-9A-F]+\)|\{[^}]*\}), (\d), item_target::(\w+), (0x[0-9A-F]+), (0x[0-9A-F]+) \},$", h, re.M)]
    calls = {}
    for name, body in re.findall(r"^static const item_call (\w+)_calls\[\] = \{ (.*) \};$", h, re.M):
        calls[name] = [(a, parse_offsets(o)) for a, o in re.findall(r"\{ (fn_\w+), (same\(0x[0-9A-F]+\)|\{[^}]*\}) \}", body)]
    return h, fns, roots, refs, sites, calls


def root_value(expr, img, ext):
    m = re.match(r"uint32_t\(ff8_externals\.menu_callbacks\[(\d+)\]\.func\)", expr)
    if m:
        return img.u32(ext["ff8_externals.menu_callbacks"] + 8 * int(m.group(1)))
    m = re.match(r"common_externals\.execute_opcode_table\[(0x[0-9A-F]+)\]", expr)
    if m:
        return img.u32(ext["common_externals.execute_opcode_table"] + 4 * int(m.group(1), 16))
    return ext[re.match(r"uint32_t\((.+)\)", expr).group(1)]


# --- matching --------------------------------------------------------------------------------
def code_refs(img, start, size, kind):
    """Offsets and targets of calls, or of 4-byte operands pointing into code, in order."""
    out = []
    for i in img.insns(start, size):
        b = i.bytes
        if b[0] == 0xE8 and len(b) == 5:
            if kind == "call":
                out.append((i.address - start, (i.address + 5 + int.from_bytes(b[1:], "little", signed=True)) & 0xFFFFFFFF))
            continue
        if kind == "abs":
            for k in range(1, len(b) - 3):
                v = int.from_bytes(b[k : k + 4], "little")
                if img.text[0] <= v < img.text[1]:
                    out.append((i.address - start + k, v))
                    break
    return out


def masked(img, start, size):
    """Bytes of [start, start+size) at instruction boundaries, addresses and branch targets masked."""
    pat, wild = bytearray(), []
    for i in img.insns(start, size + 16):
        if i.address + i.size > start + size:
            break
        b = bytearray(i.bytes)
        w = [False] * len(b)
        if (i.group(capstone.CS_GRP_JUMP) or i.group(capstone.CS_GRP_CALL)) and len(b) >= 5:
            for k in range(len(b) - 4, len(b)):
                w[k] = True
        else:
            for k in range(len(b) - 3):
                if img.base <= int.from_bytes(b[k : k + 4], "little") < 0x3000000:
                    w[k : k + 4] = [True] * 4
        pat += b
        wild += w
    return bytes(pat), wild


def starts_like(en, f_en, img, f):
    pat, wild = masked(en, f_en, 24)
    got = img.read(f, len(pat))
    return all(w or a == b for a, b, w in zip(pat, got, wild))


def same_ref(en, img, g_en, g, off, kind, window):
    refs_en = code_refs(en, g_en, window, kind)
    k = [o for o, _ in refs_en].index(off)
    target_en = refs_en[k][1]
    refs = code_refs(img, g, int(window * 1.3) + 0x100, kind)
    rank = [t for _, t in refs_en[: k + 1]].count(target_en) - 1
    alike = [r for r in refs if starts_like(en, target_en, img, r[1])]
    if len({t for _, t in alike}) == 1:
        hits = [r for r in alike if r[1] == alike[0][1]]
        if rank < len(hits):
            return hits[rank]
    return refs[k] if k < len(refs) else (None, None)


def operands(img, start, size):
    out = []
    for i in img.insns(start, size):
        if i.group(capstone.CS_GRP_JUMP) or i.group(capstone.CS_GRP_CALL):
            continue
        for op in i.operands:
            if op.type == capstone.x86.X86_OP_IMM and i.imm_offset:
                out.append((i.address + i.imm_offset - start, i.imm_size, op.imm & ((1 << (8 * i.imm_size)) - 1), "imm"))
            elif op.type == capstone.x86.X86_OP_MEM and i.disp_offset and i.disp_size == 4:
                out.append((i.address + i.disp_offset - start, 4, op.mem.disp & 0xFFFFFFFF, "disp"))
    return out


def is_operand(img, p):
    """Is p the start of a 4-byte displacement or immediate of a real instruction?"""
    for back in range(1, 11):
        for i in md.disasm(img.read(p - back, 16), p - back, 1):
            if (i.disp_size == 4 and i.disp_offset == back) or (i.imm_size == 4 and i.imm_offset == back):
                return True
    return False


def find_unique(en, start, size, img):
    pat, wild = masked(en, start, size)
    rx = re.compile(b"".join(b"." if w else re.escape(bytes([c])) for c, w in zip(pat, wild)), re.S)
    t0, t1 = img.text
    hits = [t0 + m.start() for m in rx.finditer(img.read(t0, t1 - t0))]
    return hits[0] if len(hits) == 1 else None


# --- main ------------------------------------------------------------------------------------
def main():
    folder = pathlib.Path(sys.argv[1])
    write = "--write" in sys.argv
    header, fns, roots, refs, sites, calls = parse_header()
    imgs = {r: Image(folder / f"FF8_{r}.exe") for r, _, _ in RELEASES}
    en = imgs["EN"]
    problems = []

    # how far into each EN function the table looks
    window = collections.defaultdict(lambda: 0x40)
    for f, g, call, offs in refs:
        window[g] = max(window[g], offs[0] + 0x40)
    for s in sites:
        window[s["fn"]] = max(window[s["fn"]], s["offsets"][0] + 0x40)
    for lst in calls.values():
        for f, offs in lst:
            window[f] = max(window[f], offs[0] + 0x40)

    addr = {}
    ext = {}
    for r, vname, layout in RELEASES:
        ext[r] = evaluate_externals(imgs[r], vname)
        a = {f: root_value(e, imgs[r], ext[r]) for f, e in roots.items()}
        addr[r] = a
    # EN: follow the EN column
    for f, g, call, offs in refs:
        if offs[0]:
            addr["EN"][f] = (en.u32(addr["EN"][g] + offs[0] + 1) + addr["EN"][g] + offs[0] + 5) & 0xFFFFFFFF if call else en.u32(addr["EN"][g] + offs[0])

    new_refs = []
    for f, g, call, offs in refs:
        out = list(offs)
        for r, _, layout in RELEASES:
            if r == "EN" or not offs[0]:
                if not offs[0] and offs[layout] and addr[r].get(g):
                    o = offs[layout]
                    img = imgs[r]
                    addr[r][f] = (img.u32(addr[r][g] + o + 1) + addr[r][g] + o + 5) & 0xFFFFFFFF if call else img.u32(addr[r][g] + o)
                continue
            o, t = same_ref(en, imgs[r], addr["EN"][g], addr[r][g], offs[0], "call" if call else "abs", window[g])
            if o is None:
                problems.append(f"{r}: {f} not found from {g}")
                continue
            if r in ("JP_NV", "EN2000") and out[layout] != o:
                problems.append(f"{r}: {f} offset 0x{o:X} differs from its layout's 0x{out[layout]:X}")
            out[layout] = o
            addr[r][f] = t
        new_refs.append((f, g, call, out))

    # array bases per release: vote over sites that kept their EN offset and bytes
    bases = {}
    for r, _, layout in RELEASES:
        votes = collections.defaultdict(collections.Counter)
        for s in sites:
            if s["target"] == "constant" or not s["offsets"][0] or s["fn"] not in addr[r]:
                continue
            p = addr[r][s["fn"]] + s["offsets"][0]
            if imgs[r].read(p - 2, 2) == en.read(addr["EN"][s["fn"]] + s["offsets"][0] - 2, 2):
                votes[s["target"]][(imgs[r].u32(p) - s["native"]) & 0xFFFFFFFF] += 1
        bases[r] = {t: c.most_common(1)[0][0] for t, c in votes.items()}
        inventory = ext[r]["ff8_externals.savemap"] + (bases["EN"]["inventory"] - ext["EN"]["ff8_externals.savemap"]) if r != "EN" else bases[r]["inventory"]
        if bases[r]["inventory"] != inventory:
            problems.append(f"{r}: inventory operands do not match the savemap")
    span = collections.defaultdict(int)
    for s in sites:
        if s["target"] != "constant":
            span[s["target"]] = max(span[s["target"]], s["native"] + 4)

    new_sites = []
    for s in sites:
        out = list(s["offsets"])
        for r, _, layout in RELEASES:
            f = addr[r].get(s["fn"])
            if not f or not s["offsets"][layout] and r != "EN" and not s["offsets"][0]:
                continue
            img = imgs[r]
            expected = s["native"] if s["target"] == "constant" else (bases[r][s["target"]] + s["native"]) & 0xFFFFFFFF
            if not s["offsets"][0]:  # a release-only site: check it as written
                o = s["offsets"][layout]
            elif r == "EN":
                o = s["offsets"][0]
            else:
                o = None
                p0 = f + s["offsets"][0]
                same = int.from_bytes(img.read(p0, s["width"]), "little") == expected
                if same and img.read(p0 - 2, 2) == en.read(addr["EN"][s["fn"]] + s["offsets"][0] - 2, 2):
                    o = s["offsets"][0]
                else:
                    def rank_list(im, start, base):
                        if s["target"] == "constant":
                            return [x for x in operands(im, start, int(window[s["fn"]] * 1.3) + 0x100) if x[3] == "imm" and x[1] == s["width"] and x[2] == s["native"]]
                        return [x for x in operands(im, start, int(window[s["fn"]] * 1.3) + 0x100) if x[1] == 4 and base <= x[2] < base + span[s["target"]]]
                    en_list = rank_list(en, addr["EN"][s["fn"]], bases["EN"].get(s["target"], 0))
                    l_list = rank_list(img, f, bases[r].get(s["target"], 0))
                    k = [x[0] for x in en_list].index(s["offsets"][0])
                    if k < len(l_list) and (s["target"] == "constant" or l_list[k][2] - bases[r][s["target"]] == s["native"]):
                        o = l_list[k][0]
                if o is None:
                    if s["offsets"][layout] == 0:
                        continue  # the table says this release has no such code
                    problems.append(f"{r}: operand {s['fn']}+0x{s['offsets'][0]:X} not found")
                    continue
            found = int.from_bytes(img.read(f + o, s["width"]), "little")
            if found != expected:
                problems.append(f"{r}: operand {s['fn']}+0x{o:X} is 0x{found:X}, expected 0x{expected:X}")
            if r in ("JP_NV", "EN2000") and out[layout] != o:
                problems.append(f"{r}: operand {s['fn']}+0x{o:X} differs from its layout's 0x{out[layout]:X}")
            out[layout] = o
        new_sites.append(dict(s, offsets=out))

    new_calls = {}
    for name, lst in calls.items():
        rows = []
        for f, offs in lst:
            out = list(offs)
            for r, _, layout in RELEASES:
                if r == "EN":
                    continue
                o, t = same_ref(en, imgs[r], addr["EN"][f], addr[r][f], offs[0], "call", window[f])
                if o is None:
                    problems.append(f"{r}: call {name} in {f} not found")
                    continue
                out[layout] = o
            rows.append((f, out))
        new_calls[name] = rows

    # completeness: references to moved arrays that are neither listed nor in code left alone
    listed = {r: set() for r, _, _ in RELEASES}
    for s in new_sites:
        for r, _, layout in RELEASES:
            if s["fn"] in addr[r] and s["offsets"][layout]:
                listed[r].add(addr[r][s["fn"]] + s["offsets"][layout])
    replaced = ["fn_item_name", "fn_item_description", "fn_build_battle_items", "fn_update_battle_items", "fn_random_battle_item", "fn_reserve_battle_order"]
    baseline = None
    for r, _, _ in RELEASES:
        img = imgs[r]
        skip = [(addr[r][f], addr[r][f] + 0x400) for f in replaced if f in addr[r]]
        for start, size, why in LEFT_ALONE:
            a = start if r == "EN" else find_unique(en, start, min(size, 0x40), img)
            if a:
                skip.append((a, a + int(size * 1.3)))
        t0, t1 = img.text
        data = img.read(t0, t1 - t0)
        extra = collections.Counter()
        for k in range(len(data) - 3):
            v = int.from_bytes(data[k : k + 4], "little")
            for target, base in bases[r].items():
                if base <= v < base + span[target] and t0 + k not in listed[r] and not any(a <= t0 + k < b for a, b in skip) and is_operand(img, t0 + k):
                    extra[target, v - base] += 1
        if baseline is None:
            baseline = extra  # English: neighbouring variables and byte coincidences
        for key in set(extra) | set(baseline):
            # one-off coincidences differ between builds; more references to a listed offset do not
            listed_offsets = {s["native"] for s in sites if s["target"] == key[0]}
            if extra[key] > baseline[key] and key[1] in listed_offsets:
                problems.append(f"{r}: {extra[key] - baseline[key]} unlisted reference(s) to {key[0]}+0x{key[1]:X}")

    for p in problems:
        print("PROBLEM", p)
    print(f"{len(new_refs)} function references, {len(new_sites)} operands, {sum(len(v) for v in new_calls.values())} calls, "
          f"{len(RELEASES)} releases: {'OK' if not problems else f'{len(problems)} problem(s)'}")
    if write and not problems:
        def fmt(offs):
            return f"same(0x{offs[0]:X})" if len(set(offs)) == 1 else "{ " + ", ".join(f"0x{o:X}" for o in offs) + " }"
        text = header
        for (f, g, call, old), (_, _, _, new) in zip(refs, new_refs):
            text = text.replace(f"\t{{ {f}, {g}, {'true' if call else 'false'}, {fmt(old)} }},", f"\t{{ {f}, {g}, {'true' if call else 'false'}, {fmt(new)} }},", 1)
        for old, new in zip(sites, new_sites):
            a = f"\t{{ {old['fn']}, {fmt(old['offsets'])}, {old['width']}, item_target::{old['target']}, 0x{old['native']:X}, 0x{old['value']:X} }},"
            b = f"\t{{ {old['fn']}, {fmt(new['offsets'])}, {old['width']}, item_target::{old['target']}, 0x{old['native']:X}, 0x{old['value']:X} }},"
            text = text.replace(a, b, 1)
        for name in calls:
            for (f, old), (_, new) in zip(calls[name], new_calls[name]):
                text = text.replace(f"{{ {f}, {fmt(old)} }}", f"{{ {f}, {fmt(new)} }}", 1)
        HEADER.write_text(text, encoding="utf8", newline="\r\n")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
