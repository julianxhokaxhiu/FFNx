# Battle anim sets

Some battle actions do not play the animations of the model files. They bind an **anim set** stored in the exe onto the model for the length of the action: limit breaks (Renzokuken and its finishers, Shot, Duel, Angelo, No Mercy, the Laguna, Kiros, Ward and Edea limits), Quistis' Blue Magic cast, and a few others.
An anim set holds body animations, the AnimSeq sequences that play them, and the weapon animations.

FFNx can load each anim set from a file instead. This lets a mod change these animations with no size limit, for example to ship animations converted to a higher frame rate, which no longer fit in the exe.

Mods only ship the files of the sets they change, and every set without a file stays vanilla.

## File path

```
<direct_mode_path>/exe/effect_NNN.animset
<direct_mode_path>/exe/effect_NNN_K.animset
```

Like the other [EXE data](../../mods/exe_data.md) files, `<direct_mode_path>/<language>/exe/` takes precedence when the file exists there.

- `<direct_mode_path>` is the `direct_mode_path` option of `FFNx.toml` (`direct` by default), inside the game folder.
- `NNN` is the effect id of the action, zero padded to 3 digits: the same id as the [battle effect cameras](effect_camera.md) (`effect_188.bcam` and `effect_188.animset` both belong to Normal Shot).
- Most effects bind one set, named `effect_NNN.animset`. Effects that bind several sets (one per model) name the first one `effect_NNN.animset`, then `effect_NNN_1.animset`, `effect_NNN_2.animset`, in the order the exe stores them.

Anim sets are the same in every language and in every release, so one file in `direct/exe/` works everywhere.

Files are read the first time the set is used in a battle, and dropped when the battle ends: a changed file is used from the next battle.

## Sets

| Files | Effects | Model |
|---|---|---|
| `effect_071`, `072`, `073`, `074`, `184`, `229`, `230`, `260`, `275`, `297`, `299`, `300`, `305`, `306`, `307`, `317` | Blue Magic: Laser Eye, Aqua Breath, Mighty Guard, LV? Death, Ultra Waves, Homing Laser, Fire Breath, Shockwave Pulsar, Electrocute, White Wind, Micro Missiles, Bad Breath, Gatling Gun, Degenerator, Ray-Bomb, Acid | Quistis |
| `effect_141`, `160`, `259`, `332`, `333` | Renzokuken | Squall |
| `effect_162`, `163`, `164`, `165` | Fated Circle, Rough Divide, Blasting Zone, Lion Heart | Squall |
| `effect_159`, `effect_161`, `effect_334` to `effect_343` (no `338`) | Renzokuken on a boss made of several models (159 X-ATM092, 161 Elnoyle/Elvoret) | `effect_NNN` Squall, `effect_NNN_1` (and `effect_339_2`) the target models |
| `effect_188`, `192`, `193`, `194`, `195`, `196`, `197`, `198` | Shot: Normal, Scatter, Dark, Flame, Canister, Quick, Armor, Hyper Shot | Irvine |
| `effect_274`, `effect_112`, `212`, `215`, `216` | Duel, Meteor Barret, Burning Rave, My Final Heaven, Different Beat | Zell |
| `effect_084`, `086`, `087`, `088`, `089`, `092` | Angel Wing, Angelo Cannon, Angelo Strike, Invincible Moon, Wishing Star, Angelo Search | Rinoa |
| `effect_200` | No Mercy | Seifer |
| `effect_228` | Ice Strike | Edea |
| `effect_261`, `262`, `263` | Desperado, Blood Pain, Massive Anchor | Laguna, Kiros, Ward |

The victory poses (`r0win.dat`) and the GF summon files also bind anim sets, but they are already read from the game files, so they are not concerned.

## File format

The file is an anim set exactly as the exe stores it. All values are little-endian, and every offset is relative, so a set can be copied out of the exe as it is.

| Offset | Size | Value |
|---|---|---|
| `0x00` | u32 | section count `N`: 3, or 2 for the models without a weapon (Zell, Kiros, Edea, the boss models) |
| `0x04` | u32 × N | offset of each section, from the start of the file |
| `0x04 + 4 × N` | u32 | file size |

The sections are:

1. Body animations, the same format as section 3 of a battle model `.dat` file: u32 animation count, u32 offset of each animation (from the section start), then the animations. Each animation starts with its frame count (at least 1).
2. AnimSeq sequences, the same format as the sequence section of a battle model `.dat` file: u16 sequence count, u16 offset of each sequence (from the section start, 0 for an empty sequence), then the byte code.
3. Weapon animations, same format as the body animations. Only used when the model has a weapon.

A file is ignored, and the vanilla set used, when:

- its section count is not the one of the vanilla set it replaces;
- the first section does not start right after the header, the last offset is not the file size, or a section ends before it starts;
- a section has no entry, or an offset points outside its section (or into its offset table), or an animation has 0 frames.

Set `trace_battle_animation = true` in `FFNx.toml` to log which files are used, and why a file is ignored.

The animations and sequences can be changed freely: more animations, more frames, longer sequences. They play on the model of the action, so the animations must use the bone count of that model, and the sequences may only start animations that exist in the set. (Massive Anchor, `effect_263`, also carries 8 body animations made for a 2-bone model after Ward's 8: its sequence never plays them.)

FF8UltimateEditor writes every vanilla set as these files from any FF8 exe (Fujin, "Export anim sets"), as they are or converted to 30 fps, and edits them on the character's model (Ifrit, "Exe - Limit anim." tab).

## How it works

The exe binds a set by giving its address. FFNx recognizes the set from the effect that is running and the vanilla contents of the set (its size and a checksum), so it works on every release without any address. A set the exe was patched to change is not recognized, and stays as it is.

When a recognized set has a valid file, FFNx binds the file contents instead, and keeps them until the end of the battle.
