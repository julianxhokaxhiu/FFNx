# DevTools

If you're a modder and you want to test some of your changes faster than usually playing the game, this section may be of your interest.

FFNx brings a whole new game changer tooling that will allow you to explore more of the game engine, while playing everytime you prefer.

In order to use it, you must enable the relative flag on your [`FFNx.toml` config file](https://github.com/julianxhokaxhiu/FFNx/blob/master/misc/FFNx.toml#L259).

## Field Debug

This tool will allow you to jump from field to field. It is actually supported on both FF7 and FF8.

On FF8, you can also set the game moment while on a field. Choose a known moment
from the searchable list (showing its ID, disc and location), or enter a custom
Moment ID from 0 to 65535, then click **Set Moment** to apply it. The current
game moment is displayed separately so you can verify the change.

On FF7, enter a **Progress ID** from 0 to 65535 and click **Set Progress** to
change the main game progress while on a field. The current value is shown as
**Game Progress**. FF7 does not currently have a named progress preset list.

Setting a moment or progress value does not change the field, disc or other story flags. Use
**Switch Field** separately if needed. Changing story progression can leave the
game in an inconsistent state; use a backup save for testing.

## World Debug

This will will allow you to futher investigate the World geometry and much more. Currently supported on FF8 only.
