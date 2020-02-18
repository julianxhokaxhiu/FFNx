<div align="center">
  <img src=".screens/ff7.png" alt="Final Fantasy VII running on Vulkan!">
  <br><strong><small>Final Fantasy VII running on Vulkan!</small></strong>
</div>

---

# FFNx

Next generation driver for Final Fantasy VII and Final Fantasy VIII

## Introduction
Welcome to FFNx project. This is an attempt to move forward what you were used to know as [FF7_OpenGL](https://github.com/Aali132/ff7_opengl) driver, [made by Aali](http://forums.qhimm.com/index.php?topic=14922.0).

Some of the improvements that you can find on FFNx are:
- One single .dll file to be copied
- Support for newest video and audio codecs ( WEBM, H.265, etc.)
- In-Game music OGG files playback WITH LOOP support!
- Five different Rendering Backends ( Vulkan, OpenGL, DirectX 9/11/12)

## Tech Stack
If you're curious to know it, FFNx is made with:
- C++ code base built on latest MSVC ( [Visual Studio 2019](https://visualstudio.microsoft.com/vs/features/cplusplus/) )
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

#### 1998 Official Eidos Release
1. Install the game on this path: `C:\Games\Final Fantasy VII`
2. Update your game to v1.02 ( https://www.gamefront.com/games/final-fantasy-7-advent-children/file/final-fantasy-7-retail-v1-02-patch )
2. Download the latest release here: https://github.com/julianxhokaxhiu/FFNx/releases
3. Extract the ZIP content next to where your `ff7.exe` file lives
4. Double click on [`FFNx.reg`](misc/FFNx.reg)
5. Click on Yes.
6. Enjoy!

#### Steam Official Release
1. Convert the official Steam release to the 1998 Eidos Release ( see https://forums.qhimm.com/index.php?topic=15520.0 )
2. Follow now the official 1998 release installation instructions starting from point 3

### Final Fantasy VIII

#### 2000 Official Squaresoft Release
1. Install the game on this path: `C:\Games\Final Fantasy VIII`
2. Update your game to v1.2 ( search for `ff8_1.2.zip` or `ff8_1.2G.zip` here http://forums.qhimm.com/index.php?topic=12909.msg180223#msg180223 )
2. Download the latest release here: https://github.com/julianxhokaxhiu/FFNx/releases
3. Extract the ZIP content next to where your `ff8.exe` file lives
4. Enjoy!

## Tweak the configuration
If you want a more advanced experience, for example using another backend renderer ( Vulkan instead of OpenGL, or DirectX 11 ) feel free to change the driver configuration file [FFNx.cfg](misc/FFNx.cfg).

## Join us on Discord
If you want to join our community, you can find us on Discord: https://discord.gg/N6M6pKS

## License
See [LICENSE](LICENSE).
