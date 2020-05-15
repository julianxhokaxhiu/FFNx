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

Some of the improvements that you can find on FFNx are:
- FF7/FF8: [/LARGEADDRESSAWARE](https://docs.microsoft.com/en-us/cpp/build/reference/largeaddressaware-handle-large-addresses?view=vs-2019) support!

  Up to 3.5GB of space available for mods ( this requires the [4GB Patch](https://ntcore.com/?page_id=371) in your ff7.exe ).
- FF7/FF8: High DPI support!
- FF7/FF8: Up to 16x Anisotropic support!
- FF7/FF8: Up to 16x Antialiasing support!
- FF7/FF8: Steam support! No Game converter required.
- FF7/FF8: Steam savegame preservation ( you will no more loose saves created with FFNx! )
- FF7: eStore support! No Game converter required.
- FF7: XInput controller support (Xbox 360 and compatible ones) with D-Pad working out-of-the-box!
- FF7: Vertical centering for Fields and Movies
- FF7: Fullscreen Battle scenes
- FF7: Menu cursor on the middle of words vertical alignment
- FF7: Movies will continue to play if the window game loses focus ( in window mode )
- FF7: Movies volume will respect global sound volume
- FF7: Steam sound and music volume configuration preservation ( configure at your pleasure and on the next run it will be inherited )

And, on top of all of this you will also find:
- One single .dll file to be copied
- Support for 7h 1.x/[2.x](#how-to-install-on-7h-2x)
- Support for newest video and audio codecs ( WEBM, H.265, etc.)
- VGMStream built-in library WITH LOOP support for .ogg files!
- Four different Rendering Backends:
  - OpenGL ( safe default )
  - DirectX 11 ( suggested alternative to OpenGL )
  - DirectX 12 ( experimental! )
  - Vulkan ( experimental! )

## Modding
In order to enhance the modding experience, FFNx does also provide:
- Game rendering inspection through [RenderDoc](https://renderdoc.org/)!
- [DDS Texture support](https://beyondskyrim.org/tutorials/the-dds-texture-format) up to BC7 format, with PNG Texture support as fallback.
- Support for configurable external textures path using [mod_path](misc/FFNx.cfg#L100)
- Support for an override layer of the data directory using [override_path](misc/FFNx.cfg#L116)
- Support for Winamp [Input](https://winampheritage.com/plugins/Input-6) and [Output](https://winampheritage.com/plugins/Output-7) plugins ( useful to play for eg. [PSF](http://www.vgmpf.com/Wiki/index.php?title=PSF) sound files )
- Support for [Hext](https://forums.qhimm.com/index.php?topic=13574.0) patching files inside of the [hext_patching_path](misc/FFNx.cfg#L113)

## Tech Stack
If you're curious to know it, FFNx is made with:
- C++ code base
- Latest MSVC available on [Visual Studio 2019 Community Edition](https://visualstudio.microsoft.com/vs/features/cplusplus/)
- [CMake](https://cmake.org/) ( as make files )
- [BGFX](https://github.com/bkaradzic/bgfx) ( as backend renderer )
- [BIMG](https://github.com/bkaradzic/bimg) ( for custom textures )
- [FFMpeg](https://www.ffmpeg.org/) with H/W Accelleration support
- [VGMStream](https://github.com/julianxhokaxhiu/vgmstream) using FFMpeg as backend, [with loop support for Ogg files](https://github.com/julianxhokaxhiu/vgmstream/commit/249afed15176ba254c73055e8c5124b7e7cd4b95)
- [libconfuse](https://github.com/julianxhokaxhiu/libconfuse) ( for the configuration management )
- [StackWalker](https://github.com/JochenKalmbach/StackWalker) ( for stack traces in the log file )
- [pugixml](https://github.com/zeux/pugixml) ( for the Steam XML manifest )
- [md5](http://www.zedwood.com/article/cpp-md5-function) ( for the Steam XML manifest )

## Canary vs Latest Release
When you access the releases page, you will see two available download options:
- [**Canary:**](https://github.com/julianxhokaxhiu/FFNx/releases/tag/canary) the latest snapshot of the code. Like a nightly, but untested.
- [**Latest Release:**](https://github.com/julianxhokaxhiu/FFNx/releases/latest) the official release, which is tested and should be stable enough for long gameplay sessions.

## How to install
In either way, in order to use this driver you MUST have a legal copy of the game. DO NOT ask for a copy unless bought legally.

### Final Fantasy VII

**Supported Languages:** EN, DE, FR, SP, JP*

> *: Japanese support is currently work in progress. The game starts fine but font is not rendering properly and battles do crash sometimes.

#### [1998 Eidos Release](https://www.mobygames.com/game/windows/final-fantasy-vii)
1. Install the game on this path: `C:\Games\Final Fantasy VII`
2. Update your game to v1.02 ( https://www.gamefront.com/games/final-fantasy-7-advent-children/file/final-fantasy-7-retail-v1-02-patch )
2. Download the latest `FFNx-FF7_1998` release here: https://github.com/julianxhokaxhiu/FFNx/releases
3. Extract the ZIP content next to `ff7.exe` file
4. Double click on [`FFNx.reg`](misc/FF7.reg)
5. Click on Yes.
6. Enjoy!

#### [2013 Steam Release](https://store.steampowered.com/app/39140/FINAL_FANTASY_VII/)
0. Install the game using Steam Client
1. Make sure you run at least your game once ( until the new game screen )
2. Open the installation directory of the game ( see [How to access game files](https://steamcommunity.com/sharedfiles/filedetails/?id=760447682) )
3. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
4. Extract the ZIP content next to your `ff7_*.exe` file ( for eg. for EN language `ff7_en.exe`)
5. Replace all files when asked.
6. Enjoy!

#### [2013 eStore Release](http://www.jp.square-enix.com/ffvii-pc-jp/)
1. Install the game using eStore installer.
2. Open the installation directory of the game
3. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
4. Extract the ZIP content next to your `ff7_*.exe` file ( for eg. for EN language `ff7_en.exe`)
5. Replace all files when asked.
6. Enjoy!

#### [Android Release](https://play.google.com/store/apps/details?id=com.square_enix.android_googleplay.FFVII)
1. Install the game in your Android device.
2. Locate the OBB file ( usually in `Android/obb` or `/obb` in your internal storage )
3. Save the OBB file in your Windows desktop
4. Rename the OBB file extension from `.obb` to `.zip` and extract it
5. In the extracted folder, go to `ff7_1.02` directory
6. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
7. Extract the ZIP content next to the `ff7_*.exe` files
8. Update `FFNx.cfg` flags with these values:
```
ffmpeg_video_ext = webm
external_music_path = data/music_2
external_music_ext = akb
```
9. You can now run any `ff7_*.exe` file you prefer. Enjoy!

### Final Fantasy VIII

**Supported Languages:** EN, DE, FR, SP, IT, JP

#### [2000 Squaresoft Release](https://www.mobygames.com/game/windows/final-fantasy-viii)
1. Install the game on this path: `C:\Games\Final Fantasy VIII`
2. Update your game to v1.2 ( search for `ff8_1.2.zip` or `ff8_1.2G.zip` here http://forums.qhimm.com/index.php?topic=12909.msg180223#msg180223 )
2. Download the latest `FFNx-FF8_2000` release here: https://github.com/julianxhokaxhiu/FFNx/releases
3. Extract the ZIP content next to `ff8.exe` file
4. Double click on [`FFNx.reg`](misc/FF8.reg)
5. Enjoy!

#### [2013 Steam Release](https://store.steampowered.com/app/39150/FINAL_FANTASY_VIII/)
0. Install the game using Steam Client
1. Make sure you run at least your game once ( until the new game screen )
2. Open the installation directory of the game ( see [How to access game files](https://steamcommunity.com/sharedfiles/filedetails/?id=760447682) )
3. Download the latest `FFNx-Steam` release here: https://github.com/julianxhokaxhiu/FFNx/releases
4. Extract the ZIP content next to your `ff8_*.exe` file ( for eg. for EN language `ff8_en.exe`)
5. Replace all files when asked.
6. Enjoy!

## How to install on 7h 2.x
> **WARNING:** This method is NOT SUPPORTED officially by 7h team. Please use at your own risk. DO NOT approach 7h team if something is not working properly.

1) Go to your 7h installation path
2) Go to `Resources\Game Driver`
3) Rename `7H_GameDriver.dll` to `_7H_GameDriver.dll` (only rename it; this file should NOT simply be replaced with the `FFNx.dll` from step 6)
4) Download the latest `FFNx-FF7_1998` release here: https://github.com/julianxhokaxhiu/FFNx/releases
5) Extract the ZIP content next to `ff7.exe` file
6) Rename `FFNx.dll` to `7H_GameDriver.dll` (do NOT add a copy in the `7H\Resources\Game Driver folder`)

Click Play and enjoy!

## Tweak the configuration
If you want a more advanced experience, for example using another backend renderer ( Vulkan instead of OpenGL, or DirectX 11 ) feel free to change the driver configuration file [FFNx.cfg](misc/FFNx.cfg).

## Inspect logs
If you want to check what is going on behind the scene, or you may happen to have a crash, feel free to check the `FFNx.log` file.

## Join us on Discord
If you want to join our community, you can find us on Discord: https://discord.gg/N6M6pKS

## Credits

This project could have not been a reality if those people would have not worked on FF7 and FF8 with their deep passion and knowledge.
FFNx makes use also of their work, and I will never be enough grateful to those people. The order is purely Alphabetical.

These people are:
- [Aali](http://forums.qhimm.com/index.php?action=profile;u=2862):
  - for the original Driver code FFNx is based on.
- [Chrysalis](http://forums.qhimm.com/index.php?action=profile;u=674):
  - for the battle fullscreen hext patch
  - for the menu cursor vertical centering in menu
- [DLPB](https://forums.qhimm.com/index.php?action=profile;u=6439):
  - for original Hext concept and specification that FFNx implemented as well
  - for the field vertical centering hext patch, , which FFNx provides a default patch for
- [ficedula](http://forums.qhimm.com/index.php?action=profile;u=68):
  - for 7h 1.x which FFNx provides support for
- [Iros](https://forums.qhimm.com/index.php?action=profile;u=21785):
  - for 7h 1.x which FFNx provides support for
- [Maki](http://forums.qhimm.com/index.php?action=profile;u=23937):
  - for FF8 UV Hext Patch in the world map, which FFNx provides a default patch for
  - for the help in getting the first iterations of FFNx running on FF8 2000/2013 release
- [myst6re](http://forums.qhimm.com/index.php?action=profile;u=4778):
  - for the great tools like Makou Reactor, Deling and many others he did which helped a lot in improving FF8 while working on the code
  - for the great help in the code implementing the Winamp layer being present inside FFNx
  - for the heavy testing and lifting of a lot of bugs being catched in FFNx, for FF8
  - for the Steam savegame logic in the manifest.xml for FF8
- [quantumpencil](http://forums.qhimm.com/index.php?action=profile;u=23810) and [Nax](https://github.com/nax):
  - for the original CMake files FFNx has based its work upon
  - for all the help in getting some logics wired up in the game engine and a lot of hex addresses I would never been able to figure out myself
- [Satsuki](http://forums.qhimm.com/index.php?action=profile;u=24647):
  - for the heavy testing and lifting of a lot of bugs being catched in FFNx, for FF7
  - for the field vertical centering hext patch, which FFNx provides a default patch for
  - for a lot of hex addresses I would have never been able to figure out myself otherwise
- [Sebanisu](http://forums.qhimm.com/index.php?action=profile;u=22866):
  - for the help in getting the first iterations of FFNx running on FF8 2000/2013 release
  - for the heavy testing and lifting of a lot of bugs being catched in FFNx, for FF8
- [sithlord48](http://forums.qhimm.com/index.php?action=profile;u=6501):
  - for the Steam savegame logic in the manifest.xml for FF7
- [TurBoss](https://github.com/TurBoss):
  - for 7h 1.x which FFNx provides support for
- [unab0mb](https://forums.qhimm.com/index.php?action=profile;u=31071):
  - for providing quick help in integrating FFNx on top of 7h 2.x until it gets officially supported

I'm sure I forgot many others through the route. In case you feel you're missing here, feel free to open a PR! I'll be happy to include you because you deserve this.

## License
See [COPYING.TXT](COPYING.TXT).

All rights belong to their respective owners.
