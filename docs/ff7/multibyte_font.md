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

## Runtime configuration: `FFNx.multibyte.toml`

Glyph widths and field line spacing can be overridden using the `data/FFNx.multibyte.toml` file.
The file is entirely optional and the shipped copy is a comment-only template; built-in defaults
apply until an entry is enabled.

Each `[widths.page_N]` table corresponds to `jafont_1` through `jafont_6`. Keys are hexadecimal
glyph codes without the `0x` prefix. Values use the same packed byte as member 3 of `window.bin`:
`(left_padding << 5) | width`, where the low 5 bits hold width and the high 3 bits hold padding.
Only values that differ from the built-in Japanese defaults need to be listed.

```toml
line_spacing = 32.0

[widths.page_0]
"41" = 15

[widths.page_1]
"00" = 31
```

`line_spacing` controls field dialogue line advance in pixels and accepts values from 20 through
40, including fractions.

The file is hot-reloaded when its modification time changes, checked at most once per second.
Invalid edits leave the last valid configuration active. If the file or a page entry is omitted,
the built-in defaults remain active. The same overrides are available to the Japanese edition.

## What ff7_multibyte_font does NOT do

- **Name-entry screen**: the 3-mode (hiragana/katakana/eisuu) name-entry screen stays gated
  behind `ff7_japanese_edition`. A translation whose alphabet doesn't fit the stock name screen
  needs its own solution (the Arabic project keeps default names / renames via save editing).
- **Window auto-resize**: the shared field-window autosizer uses `FFNx.multibyte.toml` while this
  mode is active. Standard single-byte editions instead read the active game's `window.bin` metrics.
- **Text conversion**: FFNx only draws bytes. Reshaping/bidi (Arabic), charmap design, and
  re-encoding game files remain the translation pipeline's job.

## Quick checklist for a new translation

1. Design your charmap: assign characters to sheet/code slots, avoiding engine control codes
   (see the note above for the JP-edition `0xD9` heart quirk — usable as a glyph cell in
   multibyte mode).
2. Paint `jafont_1..6` textures (16×16 grid per sheet).
3. Re-encode game text (kernel, field, battle, world, exe strings) to your charmap.
4. Add your glyph metrics to `FFNx.multibyte.toml`.
5. Set `ff7_multibyte_font = true` and iterate on widths/linestep live in-game.
