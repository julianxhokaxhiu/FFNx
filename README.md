<div align="center">
  <img src="https://github.com/julianxhokaxhiu/FFNx/blob/master/.logo/logo_nobg.png" alt="">
  <br><strong><small>Kudos to <a href="https://www.instagram.com/aryaaiedail/">Cinzia Cancedda (aryaaiedail)</a></small></strong>
</div>
<hr>
<div align="center">
  <img src="https://github.com/julianxhokaxhiu/FFNx/blob/master/.logo/tsunamods.png" alt="">
  <br><small>FFNx is now officially part of the <strong>Tsunamods</strong> initiative!</small>
</div>

# FFNx

Next generation modding platform for Final Fantasy VII and Final Fantasy VIII ( with native Steam 2013 release support! )

## Introduction

FFNx is an attempt to move forward what you were used to know as [FF7_OpenGL](https://github.com/Aali132/ff7_opengl) driver, [made by Aali](http://forums.qhimm.com/index.php?topic=14922.0).

In general, what you can expect out of FFNx is:

- One single .dll file to be copied
- Support for 7h 1.x/2.x
- Support for newest video and audio codecs ( WEBM, H.265, etc.)
- VGMStream built-in library WITH LOOP support for .ogg files!
- Five different Rendering Backends:
  - OpenGL ( safe default )
  - DirectX 9 ( Tech Preview: Not Recommended )
  - DirectX 11 ( suggested alternative to OpenGL )
  - DirectX 12
  - Vulkan
- ...and much more!

For a more detailed feature list, feel free to check the related [Features](https://github.com/julianxhokaxhiu/FFNx/wiki/Features) wiki page.

## Documentation

Installation instructions, configuration and much more can be found at this address: https://github.com/julianxhokaxhiu/FFNx/wiki

## Screenshots

|                       Vanilla/Steam                       |
| :-------------------------------------------------------: |
| ![Final Fantasy VII running on Vulkan](.screens/ff7.png)  |
| ![Final Fantasy VIII running on Vulkan](.screens/ff8.png) |

## Tech Stack

If you're curious to know it, FFNx is made with:

- C++ code base
- Latest MSVC available on [Visual Studio 2019 Community Edition](https://visualstudio.microsoft.com/vs/features/cplusplus/)
- [CMake](https://cmake.org/) ( as make files )
- [BGFX](https://github.com/bkaradzic/bgfx) ( as backend renderer )
- [BIMG](https://github.com/bkaradzic/bimg) ( for custom textures )
- [FFMpeg](https://www.ffmpeg.org/) with H/W Accelleration support
- [VGMStream](https://github.com/losnoco/vgmstream) using FFMpeg as backend (with loop support!)
- [tomlplusplus](https://github.com/marzer/tomlplusplus) ( for the configuration management )
- [StackWalker](https://github.com/JochenKalmbach/StackWalker) ( for stack traces in the log file )
- [pugixml](https://github.com/zeux/pugixml) ( for the Steam XML manifest )
- [md5](http://www.zedwood.com/article/cpp-md5-function) ( for the Steam XML manifest )
- [libpng](http://www.libpng.org/pub/png/libpng.html) ( for a better and faster PNG texture support )
- [imgui](https://github.com/ocornut/imgui) ( to better debug the in-game engine )
- [discohash](https://github.com/cris691/discohash) ( to extract an hash from palette game textures data known as animated textures )
- [SoLoud](https://github.com/jarikomppa/soloud) ( as the audio engine used to playback audio, music or voice files )
- [openpsf](https://github.com/myst6re/openpsf) ( as the MINIPSF emulation engine to playback PSX/PS2 music files )

## How to build

Available build profiles:

- x86-Release ( default, the same used to release artifacts in this Github page )
- x86-RelWithDebInfo ( used while developing to better debug some issues )

Any other build profile is **unsupported**.

Once the project is build you can find the output in this path: `.build/bin`

### Preparation

> **Please note:**
>
> FFNx will now use vcpkg as a package manager to resolve dependencies. Failing to follow these steps will fail your builds.

0. Clone the [vcpkg](https://vcpkg.io) project in the root folder of your `C:` drive ( `git clone https://github.com/Microsoft/vcpkg.git` )
1. Go inside the `C:\vcpkg` folder and double click `bootstrap-vcpkg.bat`
2. Open a `cmd` window in `C:\vcpkg` and run the following command: `vcpkg integrate install`

### Visual Studio

> **Please note:**
>
> By default Visual Studio will pick the **x86-Release** build configuration, but you can choose any other profile available.

0. Download the the latest [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) installer
1. Run the installer and import this [.vsconfig](.vsconfig) file in the installer to pick the required components to build this project
2. Once installed, open this repository **as a folder** in Visual Studio 2019 and click the build button.

### Visual Studio Code

0. **REQUIRED!** Follow up the steps to install Visual Studio, which will also install the MSVC toolchain
1. Download and install the latest [Visual Studio Code](https://code.visualstudio.com/) release
2. Install the following extensions:
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
   - https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools
3. Open this repository as a folder in Visual Studio code
4. Choose as build profile in the status bar `CMake: [Release]` ( or one of the aforementioned profiles )
5. Click the button on the status bar `Build`

## Auto-Formatting

### CMake Files

0. **REQUIRED!** Install [Python](https://www.python.org/)
1. Install [cmake-format](https://github.com/cheshirekow/cmake_format#installation) and make sure the binary is available in your PATH environment variable
2. **OPTIONAL!** Integrate it [in your own IDE](https://github.com/cheshirekow/cmake_format#integrations) ( eg. for Visual Studio Code use [the relative extension](https://marketplace.visualstudio.com/items?itemName=cheshirekow.cmake-format) )

## Support

FFNx offers multiple support channels, pick the one you prefer

### Forums

- Qhimm Forum: http://forums.qhimm.com/index.php?topic=19970.0
- Tsunamods Forum: https://forum.tsunamods.com/viewtopic.php?p=41#p41

### Discord

- Qhimm FFNx-FF7 ( Final Fantasy VII only): https://discord.gg/N6M6pKS
- Qhimm FFNx-FF8 ( Final Fantasy VIII only): https://discord.gg/u6M7DnY
- Tsunamods FFNx: https://discord.gg/Urq67Uz ( remember to hit the Red Chocobo reaction! )

### Github

- Issues: https://github.com/julianxhokaxhiu/FFNx/issues

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
  - for the field vertical centering hext patch, which FFNx provides a default patch for
  - for the Soft-Reset original concept
  - for the no battle original concept
- [dziugo](http://forums.qhimm.com/index.php?action=profile;u=1660):
  - for the original FMV skip concept
- [ficedula](http://forums.qhimm.com/index.php?action=profile;u=68):
  - for 7h 1.x which FFNx provides support for
- [Iros](https://forums.qhimm.com/index.php?action=profile;u=21785):
  - for 7h 1.x which FFNx provides support for
- [JWP](http://forums.qhimm.com/index.php?action=profile;u=3772):
  - for imgui integration within FFNx
- [Kranmer](http://forums.qhimm.com/index.php?action=profile;u=4909)
  - for PHS save everywhere Hext patch
  - for the no battle original concept
- [Maki](http://forums.qhimm.com/index.php?action=profile;u=23937):
  - for FF8 UV Hext Patch in the world map, which FFNx provides a default patch for
  - for the help in getting the first iterations of FFNx running on FF8 2000/2013 release
- [myst6re](http://forums.qhimm.com/index.php?action=profile;u=4778):
  - for the great tools like Makou Reactor, Deling and many others he did which helped a lot in improving FF8 while working on the code
  - for the great help in the code implementing the MINIPSF emulation layer being present inside FFNx
  - for the heavy testing and lifting of a lot of bugs being catched in FFNx, for FF8
  - for the Steam savegame logic in the manifest.xml for FF8
  - for the real-time SFX volume change for FF7
- [quantumpencil](http://forums.qhimm.com/index.php?action=profile;u=23810) and [Nax](https://github.com/nax):
  - for the original CMake files FFNx has based its work upon
  - for all the help in getting some logics wired up in the game engine and a lot of hex addresses I would never been able to figure out myself
- [Satsuki](http://forums.qhimm.com/index.php?action=profile;u=24647):
  - for the heavy testing and lifting of a lot of bugs being catched in FFNx, for FF7
  - for the field vertical centering hext patch, which FFNx provides a default patch for
  - for a lot of hex addresses I would have never been able to figure out myself otherwise
  - for the original Speedhack concept and help in getting it natively into FFNx
- [Sebanisu](http://forums.qhimm.com/index.php?action=profile;u=22866):
  - for the help in getting the first iterations of FFNx running on FF8 2000/2013 release
  - for the heavy testing and lifting of a lot of bugs being catched in FFNx, for FF8
- [sithlord48](http://forums.qhimm.com/index.php?action=profile;u=6501):
  - for the Steam savegame logic in the manifest.xml for FF7
- [TurBoss](https://github.com/TurBoss):
  - for 7h 1.x source code release and FFNx enablement
- [unab0mb](https://forums.qhimm.com/index.php?action=profile;u=31071):
  - for the official integration of FFNx within 7thHeaven 2.3+

I'm sure I forgot many others through the route. In case you feel you're missing here, feel free to open a PR! I'll be happy to include you because you deserve this.

## License

FFNx is released under GPLv3 license, and you can get a copy of the license here: [COPYING.txt](COPYING.txt)

If you paid for it, remember to ask for a refund to the person who sold you a copy. Make also sure you get a copy of the source code if you got it as a binary only.

If the person who gave you a copy will refuse to give you the source code, report it here: https://www.gnu.org/licenses/gpl-violation.html

All rights belong to their respective owners.
