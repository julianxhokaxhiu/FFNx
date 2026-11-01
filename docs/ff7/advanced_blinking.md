# Advanced field blinking and facial expressions

FFNx's advanced blinking system lets Final Fantasy VII field models use custom eye and mouth textures. Modders can add expressions such as closed eyes, a smile, or a talking mouth, then select them through field scripts.

This feature is for **field models**, not battle models or faces in movie files. Enabling it does not automatically add expressions or lip-sync: your mod must supply the artwork and any scripts that animate it.

Field models can still use scripted expressions during movie playback, including scenes with models drawn over a movie background. This does not alter faces recorded in the movie itself.

## What you need

- FFNx with advanced blinking enabled.
- A compatible field model.
- FF7 `.TEX` textures for the expressions you want to use.
- Edited field scripts if you want expressions beyond ordinary blinking.

Original field models retain their normal blinking behavior. Replacement models must be prepared for advanced blinking, with compatible eye and mouth parts and texture mapping. For custom mouths, the model must have a separately replaceable mouth; a mouth painted into the main face texture cannot be animated independently just by adding files.

Design expression textures to fit the target model's texture layout. Use a field-model editing tool to prepare replacement models and a texture tool that can export FF7 `.TEX` files. Renaming a PNG or DDS file to `.TEX` does not convert it.

## Enable the feature

Set this option in `FFNx.toml`:

```toml
ff7_advanced_blinking = true
```

Expression files go in the `flevel` folder under the configured `direct_mode_path`. With the default:

```toml
direct_mode_path = "direct"
```

install them here:

```text
<Final Fantasy VII installation>\direct\flevel\
```

These expression files use [Direct mode](../mods/direct_mode.md), not the regular `mod_path` external-texture folder. If distributing through a mod manager, arrange your package so that the files are available at the equivalent Direct mode location.

## Name your expression textures

The easiest approach is to use model-specific filenames:

| Expression | Filename |
| --- | --- |
| Left eye | `eye_<model>_<index>.TEX` |
| Right eye | `eye_<model>r_<index>.TEX` |
| Mouth | `mouth_<model>_<index>.TEX` |

`<model>` is the field model's HRC filename without the extension, **not** the character's display name or the field's name. For example, use `AAAA` for `AAAA.HRC`.

`<index>` is the expression number selected by the script, written in decimal without leading zeros. Use the exact naming shown above, including the `r` immediately after the model name for the right eye.

### Example: an expression for AAAA.HRC

Provide:

```text
direct\flevel\eye_AAAA_3.TEX
direct\flevel\eye_AAAAr_3.TEX
direct\flevel\mouth_AAAA_1.TEX
```

Then select left eye `3`, right eye `3`, and mouth `1` in the model's field script.

To supply custom textures for the ordinary closed-eye state, use:

```text
direct\flevel\eye_AAAA_2.TEX
direct\flevel\eye_AAAAr_2.TEX
```

Different field costumes or model variants can have different HRC names. Supply files for each model you intend to support, or use the shared fallbacks below.

## Select expressions in field scripts

Use the **EYETX** command, which is KAWAI subcommand `00`, in the actor's field script. It supplies three values: left eye, right eye, and mouth.

- Eye values `0`, `1`, and `2` retain the game's normal eye-control behavior. `2` is the closed-eye state.
- Use values `3` through `255` for additional custom eye expressions. Supply both eye textures and select an extended value for both eyes for predictable results.
- Mouth values `0` through `255` select the matching mouth texture. You decide what each number looks like; there is no built-in list of smiles, speech shapes, or emotions.
- Mouth `0` can have a custom texture too. If you want a neutral expression, provide one or leave that texture absent to use the model's original mouth.

For talking animation, sequence mouth indices with suitable waits in the field script. FFNx does not choose speech shapes or synchronize them to dialogue automatically.

Restore the normal eye settings and your chosen neutral mouth after an expression sequence. Set the intended expression again when entering a different field rather than relying on the previous field's facial state.

## Optional shared textures

Model-specific files take priority. If one is missing, FFNx looks for a shared texture:

| Expression | Shared filename |
| --- | --- |
| Character left eye | `<default-left-eye-basename>_<index>.TEX` |
| Character right eye | `<default-right-eye-basename>_<index>.TEX` |
| Character mouth | `<character-prefix>_mouth_<index>.TEX` |
| Generic NPC left eye | `npc_eye2_<index>.TEX` |
| Generic NPC right eye | `npc_eye2r_<index>.TEX` |
| Generic NPC mouth | `npc_mouth_<index>.TEX` |

For example, Cloud's shared eye basenames are `c_eye2` and `c_eye2r`, and his mouth prefix is `c`:

```text
direct\flevel\c_eye2_3.TEX
direct\flevel\c_eye2r_3.TEX
direct\flevel\c_mouth_1.TEX
```

Use the character's default eye texture names as the reference for shared naming. Generic NPCs use the `npc` names instead.

When no matching custom eye texture exists, the system falls back to the default eye texture. When no matching mouth texture exists, it uses the model's original mouth. You do not need to provide every possible expression number.

## Testing and troubleshooting

1. Confirm `ff7_advanced_blinking = true`.
2. Confirm your files are under `<direct_mode_path>\flevel`, with valid `.TEX` content.
3. Check the exact HRC basename and expression numbers in your filenames.
4. Test ordinary blinking as well as your scripted expressions.
5. If mouths do not change, confirm the model supports a separately replaceable mouth.
6. If artwork appears stretched or misplaced, check its compatibility with the model's texture mapping.
7. Check `FFNx.log` for facial texture loading errors.

After adding or replacing expression files while testing, restart the game or move to a different field and return before checking the result. Files are not refreshed continuously while you remain in the same field.
