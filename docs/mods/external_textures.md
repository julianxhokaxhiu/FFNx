# External textures

You can override game's textures with images in DDS (recommended) or PNG format.
To know where to put your custom textures, set the options `trace_loaders` and `show_missing_textures` to `true` and look at the content of FFNx.log
when the game is running.

It is also possible to dump textures on runtime in PNG format by setting the option `save_textures` to true.

By default, FFNx looks for textures relatively to the `./mods/Textures` directory, this path can be changed via the option `mod_path`.

## FF8

[List of external textures](../ff8/mods/external_textures.md)

### FF8 Remastered archive textures

Textures loaded from the ZZZ archives can also be overridden. Keep the archive-relative
path, including the `textures` directory, under `mod_path`. For example, with the default
configuration, `textures/battle.fs/hd_new/D4C009_1.png` can be replaced by:

```text
mods/Textures/textures/battle.fs/hd_new/D4C009_1.png
```

The same layout is supported under `override_mod_path`, which is checked before `mod_path`.
Within each path, formats are tried in the order configured by `mod_ext`, so a DDS replacement
uses the same filename with a `.dds` extension. If no replacement loads, the original archive
PNG is used. Existing Classic texture overrides retain their normal lookup precedence.

Keep the Remastered texture's atlas layout and proportions, including any padding or multiple
images on the same sheet. Enable `trace_loaders` to see which external file replaces each
`zzz://textures/...` request. Remastered HD textures must be enabled for the relevant category.
