# FAQ

## Common

### I have an AMD GPU and the game has weird visual artifacts while playing

If you own an AMD GPU, chances are you have some settings in your GPU which makes the rendering not optimal or with visual artifacts.

You can try the following steps to fix ( not 100% guarantee but worth a try ):

1. Download the latest AMD Drivers https://www.amd.com/en/support
2. Install the drivers and remember to pick the option **Restore settings to Factory default** while installing
3. When opening the AMD Control Panel for the first time, remember to choose the **Standard** profile.
4. Set `renderer_backend = 4` on the [FFNx.toml](https://github.com/julianxhokaxhiu/FFNx/blob/master/misc/FFNx.toml#L20) config file.

After doing those steps, you should be able to play normally without any visible visual artifact.

## Final Fantasy VII

### Game crashed, now what?

Don't panic. FFNx creates an emergency save for you automatically, located next to your own `ff7.exe` file, located in `saves\crash.ff7`. You can rename this file to for eg. `save00.ff7` and load the first slot on the new game screen.

BE AWARE that this is an emergency save and not everything might have been saved. **Use at your own risk.**

#### If you ARE NOT using 7th Heaven / any third party mod pack

You may open an issue here: https://github.com/julianxhokaxhiu/FFNx/issues

Remember to include:
- the `FFNx.log` file you got immediately after the crash ( if you run the game again, it will be overwritten and precious information to understand the issue will be lost forever )
- the `crash.ff7` file you can find under `saves\` directory
- a description of the issue and how to replicate it
- a proof ( picture or video ) that shows the intended issue

#### If you ARE using 7th Heaven / any third party mod pack

Please approach relative Mod Authors through the support links that you can find here: https://github.com/julianxhokaxhiu/FFNx#support

DO NOT attempt to open an issue in this repository. It will be closed immediately without further explainations.

### I installed the 1998 edition from CDs but movies are not playing

In order to fix that behavior you need to:

- Find the path where `ff7.exe` is installed ( for eg. `C:\Games\Final Fantasy VII` )
- Open `RegEdit.exe` and go to this key:
  - x32: `HKEY_LOCAL_MACHINE\SOFTWARE\Square Soft, Inc.\Final Fantasy VII`
  - x64: `HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Square Soft, Inc.\Final Fantasy VII`
- Update the key named `MoviePath` value to `c:\games\final fantasy vii\Data\Movies\`
- Copy the movie files from all your CDs into `c:\games\final fantasy vii\Data\Movies\`

You should now be able to play vanilla movies as well as the one distributed via mods.

## Final Fantasy VIII

### Game crashed, now what?

You may open an issue here: https://github.com/julianxhokaxhiu/FFNx/issues

Remember to include:
- the `FFNx.log` file you got immediately after the crash ( if you run the game again, it will be overwritten and precious information to understand the issue will be lost forever )
- a save file of the nearest location required to replicate the issue
- a description of the issue and how to replicate it
- a proof ( picture or video ) that shows the intended issue

### After installing the game on Windows 10, when I attempt to run it, it starts as a process but shows nothing

This might happen because your game installation might have got corrupted. There might be many issues affecting this.

One of the possible fixes you can try is:
- Pick the CD-ROM drive letter on your Windows installation ( say E: )
- Open `RegEdit.exe` and go to this key:
  - x32: `HKEY_LOCAL_MACHINE\SOFTWARE\Square Soft, Inc\FINAL FANTASY VIII\1.00`
  - x64: `HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Square Soft, Inc\FINAL FANTASY VIII\1.00`
- Update the key named `DataDrive` value to `e:`

Try to run again the game. If everything was done correctly, the game should start fine.
