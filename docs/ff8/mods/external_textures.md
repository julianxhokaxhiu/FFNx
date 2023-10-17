# List of external textures names in FF8

Texture names pattern in FF8 mostly follows the paths used in FS/FL/FI archives in the original game.

Except for the Menu module, you can add the language at the beginning of the path for localization:
`{mod_path}\fre\cardgame\cards_00.dds`

## Triple Triad

Path: `{mod_path}\cardgame`

| File name | Description             | Animated | Multi palettes |
| --------- | ----------------------- | -------- | -------------- |
| cards_00  | Cards front and back    | No       | No             |
| game_00   | Game background         | No       | No             |
| icons_00  | Numbers, icons and text | No       | No             |
| intro_00  | Intro/outro background  | No       | No             |
| font_00   | Font (unused)           | No       | Yes (2)        |

## Battle

Path: `{mod_path}\battle\{filename with extension in FS archive}-0_00`

Example: `.\mods\Textures\battle\A0STG101.X-0_00.dds`

## Worldmap

Path: `{mod_path}\world`

### wmsetxx.obj

Path: `{mod_path}\world\dat\wmset\section{section number}\texture{texture number}_00`

### texl.obj

Path: `{mod_path}\world\dat\texl\texture{texture number}_00`

## Menu

Path: `{mod_path}\data\{lang}\menu`

**Note:** this can change in the future for more consistency with other modules.

## Field

Path: `{mod_path}\field`

### Maps

One texture per map.

Path: `{mod_path}\field\mapdata\{map name}\{map name}.png`

Example: `mods\Textures\field\mapdata\bccent_1\bccent_1.png`

#### Legacy support for Tomberry's mods

There is an alternative way to override maps, but on some edge cases it can be buggy.
This is compatible with mods made for [Tomberry](https://forums.qhimm.com/index.php?topic=15945.0).

Path: `{mod_path}\field\mapdata\{first two letters of map name}\{map name}\{map name}_{number}.png`

Example: `mods\Textures\field\mapdata\bc\bccent_1\bccent_1_0.png`

### Models

Path: `{mod_path}\field\model`

 - main_chr: textures that come from field/model/main_chr.fs subarchive
 - second_chr: textures that come from field/mapdata/.../chara.one files,
   merged together in one directory and named with theirs original identifiers (o001 for object, p001 for persona)
