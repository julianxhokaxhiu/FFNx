# Direct mode

The Direct mode allows modders to bypass game archives
(LGP for FF7 or FS/FI/FL for FF8) and read files directly from the directory
pointed by the `direct_mode_path` configuration entry.

For example: if FF7 is looking for aaab.rsd in char.lgp, this mode will make it open `direct/char/aaab.rsd` first,
If this file doesn't exist it will look for the original in the LGP archive
Another example: if FF8 is looking for `c:/data/en/FIELD/mapdata/bc/bccent12/bccent12.msd` in field.fs,
this mode will make it open `direct/en/FIELD/mapdata/bc/bccent12/bccent12.msd` if it exists.
If it does not exist, it will retry without the lang directory `direct/FIELD/mapdata/bc/bccent12/bccent12.msd`

## FF8 Remastered model overrides

When running FF8 Remastered, loose `.mch` and `chara.one` model files can be placed
under `<direct_mode_path>/remastered/`, preserving their archive-relative paths.
The language-specific path is checked first, then the path without the language:

```text
direct/remastered/<lang>/<archive-relative model path>
direct/remastered/<archive-relative model path>
```

These overrides have the highest priority, including over loose classic `.mch`
files under the game's `data/` directory. They retain Remastered model scaling
and automatic Remastered HD texture selection, subject to the existing texture
settings. Use this folder for Remastered-format models, not classic models.

Other FF8 editions ignore this folder. Existing overrides outside `remastered/`
keep their classic behavior. If no Remastered model override exists, the existing
lookup order is unchanged.

## FF8 battle model chunks

A battle model file can also be replaced section by section, so a mod ships only the sections it changes
and keeps the other ones from the game or from another mod.
Name the file `<file name>.chunk.<section number>`, sections are counted from 1. For example
`direct/battle/d0c000.dat.chunk.3` replaces the section 3 (animations) of Squall's body.
As for whole files, `direct/<lang>/battle/` is looked up first, then `direct/battle/`.

This works with:
- `c0m000.dat` to `c0m143.dat` (monsters, 11 sections)
- `dXcNNN.dat` (character bodies) and `dXwNNN.dat` (weapons), with the section count of the file
