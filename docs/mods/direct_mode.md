# Direct mode

The Direct mode allows modders to bypass game archives
(LGP for FF7 or FS/FI/FL for FF8) and read files directly from the directory
pointed by the `direct_mode_path` configuration entry.

For example: if FF7 is looking for aaab.rsd in char.lgp, this mode will make it open direct/char/aaab.rsd first,
If this file doesn't exist it will look for the original in the LGP archive
Another example: if FF8 is looking for c:/data/en/FIELD/mapdata/bc/bccent12/bccent12.msd in field.fs,
this mode will make it open direct/FIELD/mapdata/bc/bccent12/bccent12.msd if it exists.

