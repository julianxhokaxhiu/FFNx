# FF8 ability regressions

These tests run the actual shared kernel load hook and retail ability-list, text, GF-stat and menu-list machine code in a private 32-bit process. They include the current production `kernel_ability.cpp` unchanged and use the game's real save structures. The C++ learned-menu-mask replacement is installed through a test implementation of FFNx's `replace_function`. No running game is attached or modified.

Requirements: Windows, Python 3, Visual Studio x86 C++ tools, a US English retail executable and vanilla `kernel.bin`. No third-party Python packages are needed for the runner. Retail game files and exported images are supplied locally and must not be committed.

```powershell
python tests/ff8/ability_menu_regression.py --exe "C:/games/FF8/FF8_EN.exe" --kernel "C:/fixtures/kernel.bin"
```

The runner finds VS with `vswhere`; `--vcvars` selects a toolchain explicitly. `--build-dir` chooses where generated fixtures, the executable and logs are kept.

## Language images

Pass `--language-images C:/fixtures/languages` to also run the complete suite on IDA-exported EN, DE, ES, FR, IT, JP and JP_NV images. Each image needs a matching `.anchors` and `.operands` file. The latter contains actual decoded absolute operands, preventing conditional-branch bytes from being mistaken for pointers in a raw byte sweep.

`export_ability_image.py` runs inside IDAPython on a **local copy** of the corresponding database. Its module docstring describes the JSON anchor record. Verify each anchor against that executable, retain the database input hash, and use the test enum indices documented there. A batch invocation is:

```text
idat.exe -A -S"tests/ff8/export_ability_image.py --config C:/fixtures/FF8_DE.json --output-dir C:/fixtures/languages --exit" C:/fixtures/FF8_DE.exe.i64
```

The exporter verifies the executable hash against the anchor record. It writes initialized program bytes and zeroes undefined bytes, without renaming functions or patching database bytes. The map format is `FF8MAP01`, a little-endian segment count, then `{u32 address, u32 size, bytes[size]}` for each segment. The anchors record is ten little-endian dwords in the order used by the exporter. The operands file is a dword count followed by dword addresses.

## Coverage

- Magic-only and combined magic/ability growth retain their separate initialization paths.
- Vanilla files do not arm the extension. Unexpected menu instructions reject the patch before readers change, and diagnostics stay silent without `trace_all`.
- A new GF ability is learnable and named correctly, remains visible after learning, and provides its summon-damage effect through retail GF stat code.
- Group insertion and 128 total IDs preserve the menu mask, including bit 31.
- Menu entries 25 and 32 are enumerated; both initialization and refresh handle all 32 IDs and leave adjacent game state untouched.
- Japanese map-seal callback locations validate along with the other language layouts.
- Malformed section offsets, partial records and removed vanilla entries do not arm any ability patch.
- Relocated ability, junction and menu pointer operands no longer reference retired buffers.

There are 16 scenarios per input. The language matrix uses seven database images plus the supplied English executable (128 runs). Only menu task allocation and the final UI handler are stubbed for the initialization scenario; list building and mask calculation execute normally. These checks do not establish graphical menu layout, a complete playthrough, all battle-command behavior, or compatibility with other mods.
