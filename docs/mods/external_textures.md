# External textures

You can override game's textures with images in DDS (recommended) or PNG format.
To know where to put your custom textures, set the options `trace_loaders` and `show_missing_textures` to `true` and look at the content of FFNx.log
when the game is running.

It is also possible to dump textures on runtime in PNG format by setting the option `save_textures` to true.

By default, FFNx looks for textures relatively to the `./mods/Textures` directory, this path can be changed via the option `mod_path`.

## FF8

[List of external textures](../ff8/mods/external_textures.md)

### Lookup order

For each texture, FFNx stops at the first image found:

1. **External overrides of `main.zzz` textures** (FF8 Remastered only), e.g. `mods/Textures/textures/battle.fs/hd_new/c0m007_0.dds`
2. **Classic external textures** (all editions), e.g. `mods/Textures/battle/C0M007.DAT-0_00.dds`
3. **The original `main.zzz` texture** (FF8 Remastered only)
4. **The texture loaded by the game engine**

Steps 1 and 3 apply only to textures that exist in `main.zzz`. Not every texture has a Remastered version; for example, many battle models have no entry in `main.zzz`. Such textures use only steps 2 and 4.

For textures that use multiple images or palettes, such as battle models, the source is chosen once for the whole texture:

- If any `main.zzz` override exists, the Remastered images and layout are used. Images that are not overridden come from `main.zzz`.
- Otherwise, if any classic external texture exists, only classic textures are used, with the classic layout. This lets a mod ship a modified model together with its classic textures without mixing in Remastered images.
- Otherwise, the original `main.zzz` images are used.

Within each step, formats are tried in the order configured by `mod_ext`. In step 1, `override_mod_path` is checked before `mod_path`. In step 2, the language subfolder (for example `eng/`) is tried before the root folder.

### FF8 Remastered textures

To replace a texture that FF8 Remastered loads from `main.zzz`, keep its full archive path, including the leading `textures` directory, under `mod_path`. Change the extension when using DDS.
This applies to any texture in the archive:

| `main.zzz` texture | External override |
| --- | --- |
| `textures/battle.fs/hd_new/<model>_<n>.png` | `mods/Textures/textures/battle.fs/hd_new/<model>_<n>.dds` |
| `textures/field.fs/field_hd_new/<model>_<n>.png` | `mods/Textures/textures/field.fs/field_hd_new/<model>_<n>.dds` |
| `textures/field.fs/field_hd/<model>_<n>.png` | `mods/Textures/textures/field.fs/field_hd/<model>_<n>.dds` |
| `textures/hires/<name>.tex/<n>.png` | `mods/Textures/textures/hires/<name>.tex/<n>.dds` |
| `textures/hires/<name>.tex/<lang>/<n>.png` | `mods/Textures/textures/hires/<name>.tex/<lang>/<n>.dds` |
| `textures/icon.tex/<lang>/<n>.png` | `mods/Textures/textures/icon.tex/<lang>/<n>.dds` |
| `textures/cards/<name>.png` | `mods/Textures/textures/cards/<name>.dds` |
| `textures/<folder>/<name>.png` | `mods/Textures/textures/<folder>/<name>.dds` |

Keep the Remastered texture's atlas layout and proportions, including any padding or multiple images on the same sheet.
If an override exists but cannot be decoded, the original archive PNG is used.
Remastered HD textures must be enabled for the relevant category.

Enable `trace_loaders` and `show_missing_textures` to log each candidate path:

- Missing step 1 and 2 candidates are logged as `Texture does not exist, skipping: ...`.
- When step 3 is reached, the `main.zzz` override candidates are checked again and logged as `Could not load external texture: ...`.
- `Zzz::lookup ... not found in main.zzz` means the archive has no Remastered version of that texture.
