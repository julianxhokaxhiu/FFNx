# FF8 item expansion regression tests

Run on Windows with an x86 MSVC toolchain and Python 3. These tests do not start
or attach to the game. The native harness reserves the retail image address range
inside its own process and executes selected retail functions there. Never build
the native harness as a DLL or inject it into another process.

From the repository root, with the x86 developer environment initialized:

```powershell
cl /nologo /std:c++20 /W4 /EHsc /Od /RTC1 tests/ff8_items/inventory_tests.cpp /Fe:inventory_tests.exe
./inventory_tests.exe
```

The inventory test exercises ID and capacity boundaries, late slots, quantities,
sort/normalization, tail-only save writes, round trips, missing/truncated blocks,
corruption, duplicate IDs and invalid quantities.

To run the full native suite and generate fixture files, use the runner:

```powershell
./tests/ff8_items/run_tests.ps1 `
  -GameExe 'C:/Games/FF8/FF8_EN.exe' `
  -Kernel 'C:/Extracted/main/kernel.bin' `
  -Mitem 'C:/Extracted/menu/mitem.bin' `
  -Price 'C:/Extracted/menu/price.bin'
```

The runner requires extracted vanilla English item sections and the 2013 English
executable. It uses an explicit build directory under `.build`, creates fixture
copies there, compiles the pure inventory test and freestanding native test, and
runs both. The executable and input assets are read only. Use `-VisualStudio` to
select an installation directory if automatic discovery selects another version.

The fixture contains 256 item definitions and 89 battle records: IDs 199-255
are Extra Potion, with original Potion field metadata; IDs 199-254 also have
Potion-derived battle records. It defines items, not starting inventory. Use an
edited test save or field ADDITEM script to obtain them during interactive game
testing. The runner/harness supplies its own inventory in memory.

The native tests execute the production source, check every patch operand, and
cover rejection before activation, later-slot native lookup/removal, high-ID
battle-order exclusion, shop staging/purchase/writeback, battle list
construction/quantity/consumption/writeback, native page wrap, final-page cursor
bounds in item use and both rearrangement lists, manual swapping, the intercepted Sort
state, native save construction and both vanilla CRC copies, accepted-load
migration, and bounded asset loading/reloads. Rendering and input presentation
still require an interactive game test.
