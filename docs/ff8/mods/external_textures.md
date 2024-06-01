# List of external textures names in FF8

Texture names pattern in FF8 mostly follows the paths used in FS/FL/FI archives in the original game.

It is recommended to use uncompressed DDS for improved loading times.

There are two types of override:
- By texture: the easiest for modders, but with a little loading penalty
- By VRAM page: the fastest, but textures are split into chunks of 256x256, and animation are more difficult to handle

The menu module is only available by VRAM page, since source textures are already splitted by VRAM page,
the game pass thoses textures directly to the GPU.

If you mod __by texture__, file names look like this:<br>
`{mod_path}\cardgame\cards_{palette index}.dds` (palette index is zero padded)<br>
If not specified, the game will always fallback to the path with palette index equals to 00:<br>
`{mod_path}\cardgame\cards_00.dds`<br>
You can add the language at the beginning of the path for localization:<br>
`{mod_path}\fre\cardgame\cards_00.dds`

Available language suffixes: eng (English), fre (French), ger (German), ita (Italian), jp (Japanese), spa (Spanish).

If you mod __by VRAM page__, file names look like this:<br>
`{mod_path}\cardgame\cards_{relative vram page}_{palette x}_{palette y}.dds`<br>
If a texture is not found, the game will always fallback to the path with zeroed palette x and palette y:<br>
`{mod_path}\cardgame\cards_{relative vram page}_0_0.dds`

When there are both files for the two types of mods, the VRAM page image takes priority over the other one.

Again, use [`trace_loaders`](../../mods/external_textures.md) option to see in the logs the paths the game try to lookup.

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

#### Section 17

Worldmap texture animations, replace `section38/texture21` and `section38/texture22`.

#### Section 38

| Section                    | Description                  | Animated       | Multi palettes |
| -------------------------- | ---------------------------- | -------------- | -------------- |
| texture0_00 -> texture7_00 | Main textures                | No             | No             |
| texture7_00                | Water texture (maybe unused) | No             | Not sure       |
| texture9_00                | Moon                         | No             | No             |
| texture10_00               | Sky                          | No             | No             |
| texture11_00               | Map                          | No             | Yes (2)        |
| texture12_00               | Circle Shadow                | No             | No             |
| texture13_00               | Square Shadow                | No             | No             |
| texture14_00               | Effect                       | No             | Not sure       |
| texture15_00               | Forest particles effect      | No             | Yes (6)        |
| texture16_00               | Sea                          | Yes            | Yes (6)        |
| texture17_00               | Shallow water                | Yes            | Yes (6)        |
| texture18_00               | Sea                          | Yes            | Yes (6)        |
| texture19_00               | River                        | Yes            | Yes (6)        |
| texture20_00 texture23_00  | Sea                          | No             | No             |
| texture21_00 texture22_00  | Beaches                      | See section 17 | No             |
| texture24_00               | Map field of view            | No             | No             |
| texture25_00               | Map character orientation    | No             | No             |
| texture26_00               | Effect                       | No             | No             |
| texture27_00               | Effect                       | No             | No             |
| texture28_00               | Effect                       | No             | No             |
| texture29_00               | Effect                       | No             | No             |
| texture30_00               | Effect                       | No             | No             |
| texture31_00               | Effect                       | No             | No             |
| texture32_00               | Effect                       | No             | No             |
| texture33_00               | Effect                       | No             | No             |
| texture34_00               | Effect                       | No             | No             |
| texture35_00               | City barrier disk 4          | No             | No             |

#### Section 39

Rails and roads.

#### Section 40

| File name   | Description           | Animated | Multi palettes |
| ----------- | --------------------- | -------- | -------------- |
| texture0_00 | Esthar extra texture  | No       | No             |

#### Section 42

Some mobile models.

| File name    | Description             |
| ------------ | ----------------------- |
| texture0_00  | BGU                     |
| texture1_00  | BGU (copy)              |
| texture2_00  | Luxury blue locomotive  |
| texture3_00  | Luxury blue wagon       |
| texture4_00  | Luxury green locomotive |
| texture5_00  | Luxury green wagon      |
| texture6_00  | Old locomotive          |
| texture7_00  | Old wagon               |
| texture8_00  | Grey Freight Wagon      |
| texture9_00  | Yellow car              |
| texture10_00 | Galbadia military car   |
| texture11_00 | Galbadia military car   |
| texture12_00 | Unknown                 |
| texture13_00 | Unknown                 |
| texture14_00 | Unknown                 |
| texture15_00 | Unknown                 |
| texture16_00 | Unknown                 |
| texture17_00 | Green car               |
| texture18_00 | Brown car               |
| texture19_00 | Blue car                |
| texture20_00 | Mint car                |
| texture21_00 | Pink car                |
| texture22_00 | Yellow Esthar car       |
| texture23_00 | Blue Esthar car         |
| texture24_00 | Unknown                 |
| texture25_00 | Lunatic Pandora         |
| texture26_00 | Lunatic Pandora         |
| texture27_00 | Lunatic Pandora         |
| texture28_00 | Disk 4 doors            |
| texture29_00 | City barrier disk 4     |
| texture30_00 | Jumbo Cactuar           |
| texture31_00 | Rocks                   |
| texture32_00 | City barrier disk 4     |

### texl.obj

Path: `{mod_path}\world\dat\texl\texture{texture number}_00`

High res version of `wmset\section38\texture{0,7}_00`. Will be loaded by the game as you travel the worldmap.

**Note:** Ideally, for a smooth experience, do not use texl, instead use `world\dat\wmset\section38\texture0_texture1_16_0_0.dds`
which is GPU accelerated.

### chara.one

Path: `{mod_path}\world\esk\chara_one\model{model number}-{texture number}_00`

| File name                  | Description      |
| -------------------------- | ---------------- |
| model0-0_00 -> model0-1_00 | Squall           |
| model1-0_00 -> model1-3_00 | Ragnarok         |
| model2-0_00 -> model3-1_00 | Chocobos         |
| model4-0_00 -> model4-1_00 | Squall (Student) |
| model5-0_00 -> model5-1_00 | Zell             |
| model6-0_00 -> model6-1_00 | Selphie          |

## Menu

Path: `{mod_path}\data\{lang}\menu`

**Note:** this can change in the future for more consistency with other modules.

## Field

Path: `{mod_path}\field`

### Maps

One texture per map.

Path: `{mod_path}\field\mapdata\{map name}\{map name}.png`

Example: `mods\Textures\field\mapdata\bccent_1\bccent_1.png`

#### Legacy support for Tonberry's mods

There is an alternative way to override maps, but on some edge cases it can be buggy.
This is compatible with mods made for [Tonberry](https://forums.qhimm.com/index.php?topic=15945.0).

Path: `{mod_path}\field\mapdata\{first two letters of map name}\{map name}\{map name}_{number}.png`

Example: `mods\Textures\field\mapdata\bc\bccent_1\bccent_1_0.png`

### Models

Path: `{mod_path}\field\model`

 - main_chr: textures that come from field/model/main_chr.fs subarchive
 - second_chr: textures that come from field/mapdata/.../chara.one files,
   merged together in one directory and named with theirs original identifiers (o001 for object, p001 for persona)
