"""Execute FF8 US retail functions against FFNx's current ability patch.

Requires Windows, VS C++ x86 tools, an FF8 US executable and vanilla kernel.bin.
Retail files are supplied locally and never copied into the repository.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import shutil
import struct
import subprocess
import tempfile


def sections(data):
    count, = struct.unpack_from('<I', data)
    assert count == 56
    offsets = struct.unpack_from('<56I', data, 4)
    return [bytearray(data[offsets[i]:offsets[i+1] if i < 55 else len(data)]) for i in range(56)]


def fixture(data, gf_extra=0, menu_extra=0):
    parts = sections(data)
    assert len(parts[16]) == 9*8 and len(parts[17]) == 24*8
    if gf_extra:
        entry = bytearray(parts[16][:8])
        entry[4:8] = bytes([10, 0, 0, 50])
        parts[16].extend(entry*gf_extra)
        for gf in range(16):
            for slot in range(21):
                index = gf*132 + 0x1B + slot*4 + 3
                if parts[2][index] >= 92:
                    parts[2][index] += gf_extra
        parts[2][0x1B+5*4+3] = 92  # Quezacotl: replace Boost with added SumMag +50%
    parts[17].extend(parts[17][-8:]*menu_extra)
    output = bytearray(data[:228])
    for i, part in enumerate(parts):
        struct.pack_into('<I', output, 4+i*4, len(output))
        output.extend(part)
    return bytes(output)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--exe', required=True, type=Path)
    parser.add_argument('--language-images', type=Path, help='Optional IDA-exported FF8_LANGUAGE.map and .anchors directory')
    parser.add_argument('--kernel', required=True, type=Path)
    parser.add_argument('--vcvars', type=Path)
    parser.add_argument('--build-dir', type=Path)
    args = parser.parse_args()
    source_dir = Path(__file__).resolve().parent
    repo = source_dir.parents[1]
    build = args.build_dir.resolve() if args.build_dir else Path(tempfile.mkdtemp(prefix='ffnx-menu-tests-'))
    build.mkdir(parents=True, exist_ok=True)
    if not args.vcvars:
        vswhere = Path(os.environ.get('ProgramFiles(x86)', 'C:/Program Files (x86)')) / 'Microsoft Visual Studio/Installer/vswhere.exe'
        found = subprocess.check_output([str(vswhere), '-latest', '-products', '*', '-requires',
            'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', '-find', 'VC/Auxiliary/Build/vcvarsall.bat'], text=True).splitlines()
        if not found:
            parser.error('VS x86 C++ tools were not found; supply --vcvars')
        args.vcvars = Path(found[0])
    (build / 'src/ff8').mkdir(parents=True, exist_ok=True)
    for name in ('kernel_ability.cpp', 'kernel_ability.h', 'save_data.h'):
        shutil.copy2(repo / 'src/ff8' / name, build / 'src/ff8' / name)
    magic_source = (repo / 'src/ff8/kernel_magic.cpp').read_text()
    loader_start = magic_source.index('static int __cdecl ff8_kernel_load_hook(')
    brace = magic_source.index('{', loader_start)
    level, end = 1, brace+1
    while level:
        level += (magic_source[end] == '{') - (magic_source[end] == '}')
        end += 1
    loader = magic_source[loader_start:end]
    definitions = []
    for name in ('VANILLA_KERNEL_SIZE','VANILLA_MAGIC_COUNT','MAGIC_ENTRY_SIZE','MAX_MAGIC_ID','KERNEL_SECTION_COUNT','KERNEL_MAGIC_SECTION','KERNEL_FIRST_TEXT_SEC','KERNEL_FILE_MAX'):
        definitions.append(re.search(r'^#define '+name+r'[^\n]*',magic_source,re.M).group(0))
    offsets = re.search(r'static const uint32_t exe_data_section_offsets\[32\] = \{.*?\n\};',magic_source,re.S).group(0)
    preamble = '\n'.join(definitions)+'\n'+offsets+'\nstatic char* ff8_kernel_stash=nullptr;\nstatic uint8_t ff8_magic_table[MAX_MAGIC_ID][MAGIC_ENTRY_SIZE];\nstatic int ff8_magic_count=VANILLA_MAGIC_COUNT;\n'
    (build / 'production_kernel_loader.h').write_text(preamble+loader+'\n')
    game_types = (repo / 'src/ff8.h').read_text()
    entry = re.search(r'struct ff8_gf_ability_entry \{.*?\n\};', game_types, re.S).group(0)
    (build / 'production_types.h').write_text('#pragma once\n'+entry+'\n')
    for name in ('ff8.h', 'patch.h', 'globals.h', 'common.h', 'log.h', 'utils.h'):
        (build / 'src' / name).write_text('#pragma once\n#include "../harness_support.h"\n')
    (build / 'src/ff8/kernel_magic.h').write_text('#pragma once\n#include "../../harness_support.h"\n')
    for name in ('ability_menu_harness.cpp', 'harness_support.h', 'retail_padding.asm'):
        shutil.copy2(source_dir / name, build / name)
    script = f'''@echo off
call "{args.vcvars.resolve()}" x86 >nul
if errorlevel 1 exit /b %ERRORLEVEL%
cd /d "{build}"
ml /nologo /c /coff retail_padding.asm
if errorlevel 1 exit /b %ERRORLEVEL%
cl /nologo /std:c++20 /W3 /Od /Gy /GS- /Oi- /Zl ability_menu_harness.cpp retail_padding.obj /Fe:ability_menu_tests.exe /link /NODEFAULTLIB /ENTRY:audit_start /SUBSYSTEM:CONSOLE /BASE:0x400000 /DYNAMICBASE:NO /OPT:NOREF /SECTION:.text,ERW kernel32.lib
exit /b %ERRORLEVEL%
'''
    (build / 'build.cmd').write_text(script)
    compiled = subprocess.run([os.environ.get('COMSPEC', 'cmd.exe'), '/c', str(build / 'build.cmd')],
                              cwd=build, capture_output=True, text=True)
    (build / 'build.log').write_text(compiled.stdout+compiled.stderr)
    if compiled.returncode:
        raise RuntimeError(compiled.stdout+compiled.stderr)
    vanilla = args.kernel.read_bytes()
    fixtures = {'vanilla':vanilla, 'gf117':fixture(vanilla, gf_extra=1),
        'gf128':fixture(vanilla, gf_extra=12), 'menu117':fixture(vanilla, menu_extra=1),
        'menu124':fixture(vanilla, menu_extra=8), 'combined128':fixture(vanilla, gf_extra=4, menu_extra=8)}
    def add_magic(data):
        parts = sections(data)
        parts[1].extend(parts[1][:60])
        output = bytearray(data[:228])
        for i, part in enumerate(parts):
            struct.pack_into('<I', output, 4+i*4, len(output))
            output.extend(part)
        return bytes(output)
    fixtures.update({'magic58':add_magic(vanilla),'magic58_gf117':add_magic(fixtures['gf117'])})
    invalid_offset = bytearray(fixtures['gf117'])
    struct.pack_into('<I', invalid_offset, 4+12*4, len(invalid_offset)+8)
    fractional_entry = bytearray(fixtures['gf117'])
    struct.pack_into('<I', fractional_entry, 4+12*4, struct.unpack_from('<I', fractional_entry, 4+12*4)[0]+1)
    shrink_group = bytearray(fixtures['gf117'])
    struct.pack_into('<I', shrink_group, 4+12*4, struct.unpack_from('<I', shrink_group, 4+11*4)[0]+19*8)
    fixtures.update({'invalid_offset':invalid_offset,'fractional_entry':fractional_entry,'shrink_group':shrink_group})
    for name, data in fixtures.items():
        (build / (name+'.bin')).write_bytes(data)
    cases = [('vanilla_inert','vanilla','vanilla'), ('guard_rejects_unknown','gf117','reject'),
        ('gf_visibility_effect_and_shifted_mask','gf117','first'),
        ('full_ability_budget','gf128','first'), ('menu_entry_25','menu117','last'),
        ('menu_entry_32','menu124','last'), ('full_budget_bit31','combined128','last'),
        ('all_32_refresh','combined128','all'), ('all_32_initialization','combined128','init-all'),
        ('shifted_initialization','gf117','init-first'), ('empty_refresh','combined128','none'),
        ('invalid_offset_rejected','invalid_offset','kernel-reject'),
        ('fractional_entry_rejected','fractional_entry','kernel-reject'),
        ('shrunk_group_rejected','shrink_group','kernel-reject'),
        ('magic_only_unchanged','magic58','magic-only'),
        ('magic_and_ability_growth','magic58_gf117','first')]
    results = []
    inputs = [('retail_EN',args.exe.resolve(),None)]
    if args.language_images:
        for image in sorted(args.language_images.glob('FF8_*.map')):
            anchors = image.with_suffix('.anchors')
            if not anchors.exists():
                raise RuntimeError(f'Missing language anchor file: {anchors}')
            inputs.append((image.stem,image.resolve(),anchors.resolve()))
    for language, image, anchors in inputs:
        for label, kernel, mode in cases:
            command = [str(build / 'ability_menu_tests.exe'), str(image), str(build / (kernel+'.bin')), mode]
            if anchors:
                command.extend([str(anchors),str(image.with_suffix('.operands'))])
            run = subprocess.run(command, capture_output=True, text=True)
            (build / (language+'-'+label+'.txt')).write_text(run.stdout+run.stderr, encoding='utf-8')
            print(('PASS' if run.returncode == 0 else 'FAIL')+': '+language+' / '+label, flush=True)
            results.append({'language':language,'case':label,'exit_code':run.returncode})
            if run.returncode:
                print(run.stdout+run.stderr, flush=True)
    report = {'source_sha256':hashlib.sha256((repo / 'src/ff8/kernel_ability.cpp').read_bytes()).hexdigest(),
        'exe_sha256':hashlib.sha256(args.exe.read_bytes()).hexdigest(), 'results':results}
    (build / 'results.json').write_text(json.dumps(report, indent=2))
    print('Logs:', build)
    raise SystemExit(any(result['exit_code'] for result in results))


if __name__ == '__main__':
    main()
