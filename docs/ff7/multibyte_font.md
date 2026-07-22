# Multibyte Font Mode (FF7)

`ff7_multibyte_font` lets a translation use the Japanese edition's multi-sheet font system on the
**English/International executable**, without pulling in the Japanese-edition-only menu scaling
and layout changes. It was built and tested against a full Arabic localization of FF7, but the
mechanism is script-agnostic: any translation that needs more glyphs than the base 256-entry
charset (Arabic presentation forms, Chinese, Korean, extended Cyrillic, ...) can use it.

## Enabling

In `FFNx.toml`:

```toml
ff7_multibyte_font = true
```

Do **not** combine it with `ff7_japanese_edition` — that flag is for the actual Japanese
executable and additionally applies JP-only menu layout/scaling that breaks non-JP menus.
`ff7_multibyte_font` installs only the shared pieces: the multi-sheet font loader, the per-character
multibyte draw path, and the text-drawing hooks (field, menu, battle, battle top-bar).

## How text is encoded

Text bytes are interpreted like the Japanese edition interprets them:

- A plain byte `0x00-0xFF` draws the glyph at that index of font sheet 1 (`jafont_1`).
- Escape byte `0xFA`, `0xFB`, `0xFC`, `0xFD` or `0xFE` switches the NEXT byte to font sheet
  2, 3, 4, 5 or 6 respectively. Two-byte sequence per extended glyph, e.g. `FA 12` = glyph
  `0x12` of `jafont_2`.

That gives up to 6 × 256 glyph cells. Your text-encoding pipeline decides which characters live
at which codes — FFNx does not impose any particular character set. Practical caveats:

- Control codes of the game's text format (`0xE0`+ range in kernel/field text: names, colors,
  new-line, new-page, variables...) keep their engine meaning. Don't place glyphs on bytes your
  target text sections use as control codes.
- `0xD9` is redirected to a heart icon from battle window D by the Japanese text support
  (a vanilla JP quirk). That redirect only applies to the Japanese edition; in multibyte mode
  `0xD9` is an ordinary glyph cell your translation may use.
- Bytes drawn through the multibyte path are recolored via the palette/color data. Mods that
  repaint icon cells expecting NOT to be recolored (button prompts, item icons) can conflict
  with glyphs you place in those cells; prefer free cells.

## Font textures

The loader reads the 6 sheets by the same names the JP edition uses:

```
<direct/mod path>/menu/jafont_1.tim (or .tex, and hi-res .dds/.png overrides work as usual)
...
<direct/mod path>/menu/jafont_6.tim
```

Each sheet is a 16×16 grid of glyph cells (256 cells per sheet). Author them exactly like any
other FFNx-replaceable menu texture — external hi-res textures are supported through the normal
texture replacement path (see [External textures](../mods/external_textures.md)).

## Per-character widths: `multibyte_widths.bin`

Glyph advance widths come from a table you can override **at runtime, without recompiling**:

- Path: `data/kernel/multibyte_widths.bin`, resolved through the standard layers — the
  `override_path` layer is checked first, then the release's data path: `data/lang-en/kernel/`
  (or the active language) on Steam/GOG/Store/2026 releases, plain `data/kernel/` on the 1998
  release.
- Format: raw `6 * 256` bytes — one byte per glyph, sheets in order 1..6, code order `0x00-0xFF`
  within each sheet.
- Byte packing: `(left_padding << 5) | width`, i.e. low 5 bits = advance width in font units
  (0-31), high 3 bits = left padding. Same packing as member 3 of `window.bin`.
- Hot-reload: FFNx re-reads the file whenever its modification time changes (checked at most
  once per second). You can tune spacing live while the game runs.

If the file is missing, built-in defaults are used.

## Field line step: `multibyte_linestep.bin`

Optional. Controls the vertical line advance of field dialogue for multibyte text:

- Path: `data/kernel/multibyte_linestep.bin` (same layered resolution as the widths file).
- Format: a 2-byte little-endian value, line step in **quarter pixels** (e.g. `128` = 32.0 px,
  `98` = 24.5 px). A 1-byte file is also accepted as whole pixels. Accepted range 80–160
  quarter-px; out-of-range values are ignored.
- Hot-reloaded the same way as the widths file.

Taller scripts (e.g. Arabic with diacritics) typically need a larger step than the Latin
default; adjust live until lines neither overlap nor float apart.

## Icon cells: `multibyte_iconmask.bin`

Optional, read once at startup. Marks sheet-1 cells that contain icon art (item/weapon type
icons, button prompts) rather than letters:

- Path: `data/kernel/multibyte_iconmask.bin` (same layered resolution as the widths file).
- Format: raw 256 bytes, one per `jafont_1` code. Non-zero = icon cell.
- Effect: marked cells are always drawn pure white instead of taking the current text color,
  so colored icon art keeps its true colors. This addresses the recolor conflict with icon
  mods noted above.

## What ff7_multibyte_font does NOT do

- **Name-entry screen**: the 3-mode (hiragana/katakana/eisuu) name-entry screen stays gated
  behind `ff7_japanese_edition`. A translation whose alphabet doesn't fit the stock name screen
  needs its own solution (the Arabic project keeps default names / renames via save editing).
- **Window auto-resize**: the JP-edition window auto-resize hook is not installed. Field files
  are expected to ship with correctly sized windows for the translated text (retranslation
  pipelines normally rebuild field files anyway).
- **Text conversion**: FFNx only draws bytes. Reshaping/bidi (Arabic), charmap design, and
  re-encoding game files remain the translation pipeline's job.

## Quick checklist for a new translation

1. Design your charmap: assign characters to sheet/code slots, avoiding engine control codes
   (see the note above for the JP-edition `0xD9` heart quirk — usable as a glyph cell in
   multibyte mode).
2. Paint `jafont_1..6` textures (16×16 grid per sheet).
3. Re-encode game text (kernel, field, battle, world, exe strings) to your charmap.
4. Generate `multibyte_widths.bin` from your glyph metrics.
5. Set `ff7_multibyte_font = true` and iterate on widths/linestep live in-game.
