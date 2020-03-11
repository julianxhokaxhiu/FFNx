<div align="center">
  <img src=".screens/ff7.png" alt="Final Fantasy VII running on Vulkan!">
  <br><strong><small>Final Fantasy VII running on Vulkan!</small></strong>
</div>
<br><br>
<div align="center">
  <img src=".screens/ff8.png" alt="Final Fantasy VIII running on Vulkan!">
  <br><strong><small>Final Fantasy VIII running on Vulkan!</small></strong>
</div>
---

# FFNx

Next generation driver for Final Fantasy VII and Final Fantasy VIII ( with native Steam 2013 release support! )

## Introduction
Welcome to FFNx project. This is an attempt to move forward what you were used to know as [FF7_OpenGL](https://github.com/Aali132/ff7_opengl) driver, [made by Aali](http://forums.qhimm.com/index.php?topic=14922.0).

Some of the improvements that you can find only on FFNx are:
- Inspect game internals using [RenderDoc](https://renderdoc.org/)!
- **Native** Steam support! No Game converter required.
- One single .dll file to be copied
- Support for 7h 1.x/2.x
- Support for configurable external textures path using [mod_path](misc/FFNx.cfg#L80)
- Support for newest video and audio codecs ( WEBM, H.265, etc.)
- In-Game music OGG files playback WITH LOOP support!
- Four different Rendering Backends:
  - Vulkan
  - OpenGL
  - DirectX 11
  - DirectX 12

## Tech Stack
If you're curious to know it, FFNx is made with:
- C++ code base
- Latest MSVC available on [Visual Studio 2019 Community Edition](https://visualstudio.microsoft.com/vs/features/cplusplus/)
- [CMake](https://cmake.org/) ( as make files )
- [BGFX](https://github.com/bkaradzic/bgfx) ( as backend renderer )
- [FFMpeg](https://www.ffmpeg.org/) 4.2.2 with H/W Accelleration support
- [VGMStream](https://github.com/julianxhokaxhiu/vgmstream) using FFMpeg as backend, [with loop support for Ogg files](https://github.com/julianxhokaxhiu/vgmstream/commit/a4155c817f709a7d75eec6b83973d2c6efae12ac)
- [libpng](http://www.libpng.org/pub/png/libpng.html) 1.6.37 ( for custom textures )
- [libconfuse](https://github.com/julianxhokaxhiu/libconfuse) 3.2.4 ( for the configuration management )

## Canary vs Latest Release
When you access the releases page, you will see two available download options:
- **Canary:** the latest snapshot of the code. Like a nightly, but untested.
- **Latest Release:** the official release, which is tested and should be stable enough for long gameplay sessions.

## How to install
In either way, in order to use this driver you MUST have a legal copy of the game. DO NOT ask for a copy unless bought legally.

### Final Fantasy VII

**Supported Languages:** EN, DE, FR, SP

#### [1998 Eidos Release](https://www.mobygames.com/game/windows/final-fantasy-vii)
1. Install the game on this path: `C:\Games\Final Fantasy VII`
2. Update your game to v1.02 ( https://www.gamefront.com/games/final-fantasy-7-advent-children/file/final-fantasy-7-retail-v1-02-patch )
2. Download the latest release here: https://github.com/julianxhokaxhiu/FFNx/releases
3. Extract the ZIP content next to `ff7.exe` file
4. Double click on [`FFNx.reg`](misc/FFNx.reg)
5. Click on Yes.
6. Enjoy!

#### [2013 Steam Release](https://store.steampowered.com/app/39140/FINAL_FANTASY_VII/)
1. Install the game using Steam Client
2. Open the installation directory of the game ( see [How to access game files](https://steamcommunity.com/sharedfiles/filedetails/?id=760447682) )
3. Download the latest release here: https://github.com/julianxhokaxhiu/FFNx/releases
4. Extract the ZIP content next to your `ff7_*.exe` file ( for eg. for EN language `ff7_en.exe`)
5. Delete existing `AF3DN.P`, and rename `FFNx.dll` to `AF3DN.P`
6. Enjoy!

### Final Fantasy VIII

**Supported Languages:** EN, DE, FR, SP, IT, JP

#### [2000 Squaresoft Release](https://www.mobygames.com/game/windows/final-fantasy-viii)
1. Install the game on this path: `C:\Games\Final Fantasy VIII`
2. Update your game to v1.2 ( search for `ff8_1.2.zip` or `ff8_1.2G.zip` here http://forums.qhimm.com/index.php?topic=12909.msg180223#msg180223 )
2. Download the latest release here: https://github.com/julianxhokaxhiu/FFNx/releases
3. Extract the ZIP content next to `ff8.exe` file
4. Double click on [`FFNx.reg`](misc/FFNx.reg)
5. Enjoy!

#### [2013 Steam Release](https://store.steampowered.com/app/39150/FINAL_FANTASY_VIII/)
1. Install the game using Steam Client
2. Open the installation directory of the game ( see [How to access game files](https://steamcommunity.com/sharedfiles/filedetails/?id=760447682) )
3. Download the latest release here: https://github.com/julianxhokaxhiu/FFNx/releases
4. Extract the ZIP content next to your `ff8_*.exe` file ( for eg. for EN language `ff8_en.exe`). **DO NOT** replace `eax.dll` when asked.
5. Delete existing `AF3DN.P`, and rename `FFNx.dll` to `AF3DN.P`
6. Enjoy!

## Tweak the configuration
If you want a more advanced experience, for example using another backend renderer ( Vulkan instead of OpenGL, or DirectX 11 ) feel free to change the driver configuration file [FFNx.cfg](misc/FFNx.cfg).

## Inspect logs
If you want to check what is going on behind the scene, or you may happen to have a crash, feel free to check the `FFNx.log` file.

## Join us on Discord
If you want to join our community, you can find us on Discord: https://discord.gg/N6M6pKS

## License
See [LICENSE](LICENSE).
