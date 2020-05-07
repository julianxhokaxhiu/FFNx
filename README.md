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
- **Native** Steam support! No Game converter required.
- **Native** eStore support! No Game converter required.
- **Native** Vertical centering for Fields and Movies on Final Fantasy VII. No mods required.
- **Native** Fullscreen Battle scenes on Final Fantasy VII. No mods required.
- **Native** Menu cursor alignment in Menu scenes on Final Fantasy VII. No mods required.
- **Native** XInput controller support ( DPad working Out-of-the-box!) for Final Fantasy VII.
- Movies will continue to play if the window game loses focus ( in window mode ) on Final Fantasy VII.
- Movies volume will respect global sound volume on Final Fantasy VII.
- Support for Winamp [Input](https://winampheritage.com/plugins/Input-6) and [Output](https://winampheritage.com/plugins/Output-7) plugins!
- Steam savegame preservation ( you will no more loose saves created with FFNx! )
- Steam sound configuration preservation ( configure at your pleasure and on the next run those will be inherited ) for Final Fantasy VII.
- One single .dll file to be copied
- Support for 7h 1.x/2.x
- Support for newest video and audio codecs ( WEBM, H.265, etc.)
- In-Game music OGG files playback WITH LOOP support!
- Four different Rendering Backends:
  - Vulkan
  - OpenGL
  - DirectX 11
  - DirectX 12

## Modding
In order to enhance the modding experience, FFNx does also provide:
- Game rendering inspection through [RenderDoc](https://renderdoc.org/)!
- [DDS Texture support](https://beyondskyrim.org/tutorials/the-dds-texture-format) up to BC7 format, with PNG Texture support as fallback.
- Support for configurable external textures path using [mod_path](misc/FFNx.cfg#L80)
- Support for [Ficedula FF7Music](http://ff8.fr/pub/FF7Music.zip)
- Support for [Hext](https://forums.qhimm.com/index.php?topic=13574.0)

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

> *: Japanese support is currently work in progress. The game starts fine but font is not rendering properly and battles do crash sometimes.*

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
   - Trying to expand your enjoyment with content mods? [Read the 7th Heaven Guide](https://github.com/julianxhokaxhiu/FFNx#FF7-7thHeaven-Steam-Conversion)

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
external_movie_ext = webm
external_music_path = data/music_2
external_music_ext = akb
```
9. You can now run any `ff7_*.exe` file you prefer. Enjoy!

*[Global FFNx.cfg Information](https://github.com/julianxhokaxhiu/FFNx#Manual-Configuration-of-FFNx)*

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

## FF7 7thHeaven Steam Conversion
    
**A Detailed 7th Heaven 2.0+ Guide With Manual FFNx Update**

**Be Absolutely Sure you have done the following before following this guide**
   - **[Purchase a LEGAL copy of Final Fantasy 7 on Steam](https://store.steampowered.com/app/39140/FINAL_FANTASY_VII/)**
   - **There is Absolutely NO SUPPORT for Pirated copies of the game**
   - **This can include certain unofficial "Language Packs" - Be Cautious**


**If you are Upgrading from a currently working 7H v2+ modded installation**
   - Skip to Step 6
   - Covers _FAQ:_ **Help! My Game Constantly Crashes Due to Texture Cache or Memory Issues!**

**If you are Starting from scratch:**

1. Install FF7 via Steam Client and/or [Verify Integrity of Game Cache](https://support.steampowered.com/kb_article.php?ref=2037-QEUH-3335)

2. [Download 7th Heaven](http://7thheaven.rocks/)

3. Open the 7H Installer and install 7th Heaven to default location `C:\7th Heaven\`

4. Open 7th Heaven and verify it found your installation of FF7 on Steam
   - If auto-detect fails, configure each field on the dialogue pop up to the correct location.

5. Click the `Play` button in the top left. 
   - Launching via 7th Heaven Client completes the following tasks **Automatically:** 
   - Copy needed files from your Steam installation, Create needed directory structure 
   - Patch `Steam-2013 FF7.exe-v1.09` Downgrading it to `1998-Eidos FF7.exe-v1.02`    
   - **Note that your Steam installation is not modified in any way. It will still function as normal.**
   - A Completed 7H Modded Install is created at `C:\Games\FF7\`
   - **Be aware;** Launching the game via 7th Heaven disables ALL Steam features and integration
   - Quit the game once you reach the title screen

6. Open the newly created 7H Modded Install directory `C:\Games\FF7\`
   - Locate the file `7H_GameDriver.dll` and RENAME this file to `_7H_GameDriver.dll.BAK`
   - Navigate to `C:\7th Heaven\Resources\Game Driver` and RENAME this file to `_7H_GameDriver.dll.BAK`
   - As you proceed from here on, Keep the following in mind.  
&nbsp;
&nbsp;  

   >**Manually Changing/Upgrading 7th Heaven is NOT SUPPORTED officially by the 7th Heaven Team.**
   
   >**Please DO NOT approach 7th Heaven Team if something is not working properly.**
   
 

7. Download FFNx. Acquire **Only** the `FFNx-FF7_1998` archive for 7H Installs! ***NOT The FFNx-Steam Release!!***
   - The [Stable](https://github.com/julianxhokaxhiu/FFNx/releases/) Release has the most testing and is the most reliable on average.
   - The [Canary](https://github.com/julianxhokaxhiu/FFNx/releases/tag/canary) Release Is the bleeding edge build and changes constantly. Stability varies.
&nbsp;  
&nbsp;  

   >Note that Canary builds are further ahead in development, but using those comes with risks, be aware of those risks and expect more problems to occur! SAVE OFTEN
   
   >Note that Canary is also a rolling-beta branch, there is only ever 1 build available at any one time. Previous versions are not hosted. 
   
   >If you plan to update Canary builds frequently KEEP BACKUPS of previous builds in case of instability or other unforeseen issues!


8. Extract the downloaded `FFNx-FF7-1998` archive to your 7H Modded Install `C:\Games\FF7\`
   - Locate the recently extracted file `ffnx.dll` and RENAME this file to `7H_GameDriver.dll`
   - Ignore the included `ffnx.reg` file, do not execute it.

9. Open 7th Heaven and click `Play` in the top left. 
   - Verify that the game launches, and reaches the title screen. Game will be in a very small window. This is normal. 
   - Quit the game.

## Manual Configuration of FFNx

**After verifying FFNx is installed correctly, you may want to change its behavior.** 

10. Navigate to the 7H Modded Install `C:\Games\FF7\` \( *Or another FFNx.dll Installation* \)
   - Locate the file `ffnx.cfg` and OPEN this file with a text editor.
   - READ though this file, and locate settings you want to change. REMOVE leading \# Symbols to set values to be read at runtime. 
   - DOUBLE CHECK for typos and removal of comment \# symbols for each change you make. Common Changes are;
     - `renderer_backend`
       - `Direct3D11` is commonly reported to perform well.
     - `fullscreen`  
       - Change to `yes` to allow fullscreen
     - `#resolution settings`  
       - If your `fullscreen = yes` leave both at 0 to use your desktop resolution, otherwise set pixel dimensions;
       - `window_size_x = 1920`  
       - `window_size_y  = 1080`
     - `resolution_scale` 
       - Experiencing high RAM usage and graphical errors using many PNG texture IRO's ? 
       - Change value to 2 for better performance, [or use DDS textures.](https://github.com/julianxhokaxhiu/FFNx#PNG-To-DDS-Conversion)
     - `preserve_aspect` 
       - Set to `no` if you want the black sidebars removed and render stretched to window size.
     - READ through the rest of the file and decide if you want to make other uncommon changes. Valid configuration options are listed.
     - SAVE the file and exit the editor.
  - [Link Back to top of Readme](https://github.com/julianxhokaxhiu/FFNx#FFNx) if not using 7H/Mods

11. Open 7th Heaven and browse/download catalog IRO's, or [manually download IRO mod packs](http://forums.qhimm.com/index.php) prioritize DDS converted packs.
   - For manual IRO Downloads, Click on `Import-Mod` Button in right sidebar to open IRO import tool. 
   - Select batch/single tab and select folder/file to import from. 
   - Activate any selected catalog mods, or manually imported mods with Toggle 
   - Configure one-by-one, clicking the settings `Cog Button` with mod selected and active. Assign category if necessary.
   - Click on `Auto-Sort` Button, 4th from the top on right side bar.

12. In 7th Heaven Window click top left `"Settings" -> "Game Launcher"` and identify / select your needed controller configuration and other personalized launch settings. Click save. 
    - `Steam KB+Swap AB/XO Gamepad` for XInput North American Button Layout Control Configuration
       - ***Help! My Playstation DS4 controller isnt working!***
          - [Use DS4Windows](http://ds4windows.com/) Select DS4W driver as preferred gamepad in devices and controllers windows control panel \(Xbox360 wireless controller\)

13. Click Play in top right and enjoy your modded FF7 Experience with 7th Heaven & Updated FFNx!
    - Consider [upgrading your PNG texture packs to DDS Format](https://github.com/julianxhokaxhiu/FFNx#PNG-To-DDS-Conversion) if using the catalog IRO's for better performance 


## Common Game Launch Problems

**A series of issues after installing / upgrading FFNx! with 7th Heaven**

   - Covers _FAQ:_ **Help! I'm getting the `Value cannot be null. Parameter name: input` Error on launch**

* This error means you have a corrupt/incorrect `7H_GameDriver.dll` Usually by doing [step 6 above incorrectly,](https://github.com/julianxhokaxhiu/FFNx#FF7-7thHeaven-Steam-Conversion) or a somehow faulty 7th Heaven Installation. Restart the process and try again 
   - Before you attempt to repair a broken installation, ensure a backup of your user data. \( if any relevant data exists \)

1. Backup your `*.iro` mods by copying `C:\Games\FF7\mods\` to a new location; Ex. `C:\7h-mods-backup\`
   - Use your backup location to import IRO files from, [during step 11 above.](https://github.com/julianxhokaxhiu/FFNx#FF7-7thHeaven-Steam-Conversion)

2. Backup your `*.ff7` save files by copying `C:\Games\FF7\save\` to a new location; Ex. `C:\FF7Saves\`
   - Copy from your backup location to `C:\Games\FF7\save\` [after step 6 and before step 13 above.](https://github.com/julianxhokaxhiu/FFNx#FF7-7thHeaven-Steam-Conversion)

3. Backup your 7th Heaven profile by copying `C:\7th Heaven\7thWorkshop\profiles\Default.xml` to a new location; Ex.`C:\7h-profile\`
   - After restoring your backup IRO files from [this sections step 1,](https://github.com/julianxhokaxhiu/FFNx#Common-Game-Launch-Problems) close 7th Heaven and copy this file back to its original location and overwrite it.
   - \( Step 3 is completely optional, and possibly not needed! Depending on changes to your IRO file set your old profile could be useless. Reconfiguration of individual IRO settings may be required regardless.  \)

4. DELETE both `C:\Games\FF7\` and `C:\7th Heaven\`

5. Uninstall FF7 through the steam client.
   - Delete `Final Fantasy VII` Folder from your `%Steam%\steamapps\common\` directory.

6. [Restart install process from beginning;](https://github.com/julianxhokaxhiu/FFNx#FF7-7thHeaven-Steam-Conversion)


## PNG To DDS Conversion

**How to Identify, Convert and Replace PNG-Based Texture Packs**
   - Covers _FAQ:_**Help! Running FFNx with many texture mods feels choppy or slow!**  

1. [Download the SYW Pack Builder](https://forums.qhimm.com/index.php?topic=19204.0)

2. Open 7th Heaven and identify the mods you are replacing. The pack builder can REPLACE the following iros in the 7th Heaven Catalog
   - `World Textures - Qhimm Catalog` <==> `Worldmap upscaled textures`
   - `Field Textures - Qhimm Catalog` <==> `Fields background upscaled textures`
   - `Battle Textures - Qhimm Catalog` <==> `Battle background upscaled textures`
   - `Spell Textures - Qhimm Catalog` <==> `Magic and combat Fx textures`
   - `Minigames - Qhimm Catalog` <==> `Minigames upscaled textures`
   - `User Interface` / `ESUI` <==> `Menu (SYW v4 HD gui)` **NOT a direct ESUI or Full "User Interface" Replacement!**

3. Open the pack builder, and configure it to your needs based on the information gained from step 2 
      - [Image on thread from step 1](https://forums.qhimm.com/index.php?topic=19204.0) is the recommended default settings
   - _FAQ:_ **Help! Pack Builder wont open! Says its a virus!** 
        - Disable your Antivirus / Windows Defender RealTime protection. Pack Builder is Safe.

4. Click on `"Make IRO from DDS Files"` Button. 
   - Recommended to use`"nothing"` compression level for best performance.
   - Be patient and wait for it to finish.

5. Open 7th Heaven, Toggle Each Active Mod being replaced to `Off`
   - *OR* outright REMOVE them by clicking the trashcan icon in right sidebar
   - *OR* Backup PNG IRO's being replaced Moving them out of `C:\Games\FF7\mods\7th Workshop`
   - Click refresh inside of 7th Heaven

6. In 7th Heaven click `"Import Mod"` Button 
   - Select `"Batch Import"` tab 
   - Select Output Folder from step 4, click OK.
      - *OR* COPY the produced IRO files from step 4 into the mods folder `C:\Games\FF7\mods\7th Workshop` 
   - Click Refresh in 7th Heaven

7. Activate and configure imported DDS IRO's

8. Click Play with mods in top left and enjoy the 40-70% increased performence over PNG!


## Manual PNG To DDS Conversion

**Optional but recommended - Mixing texture formats is not optimal for performance!** 
- Covers _FAQ:_ **I have other PNG mods that arent included in the pack builder!** 
    - Includes mods such as ESUI, Avatars, Gameplay mods with textures, etc.

9. [Download the PNG to DDS Converter tool by satsuki](http://forums.qhimm.com/index.php?topic=19701.0)
   - Extract archive to a working directory; Ex. `C:\PNG2DDS\`
   - Open `Satsuki.IroPng2Dds.exe` 
        - Again Possibly needing to disable AntiVirus RealTime Protection

10. Identify mods for conversion and disable them in 7th Heaven.
   - Open your 7th Heaven mods folder `C:\Games\FF7\mods\7th Workshop`
   - Select the mods identified previously and drag them all to the open Converter window
   - Select your conversion options.  match them to [image on the thread in step 9](http://forums.qhimm.com/index.php?topic=19701.0), 
       - Default options Illustrated are **highly** recommended!

11. Click `"convert iro to dds version"`    
   - ***Help! nothing happens or I get errors during conversion!***
   - Install the provided runtimes located in the `"runtimes"` folder of the extracted package. Run the converter as administrator in a file-writeable directory

12. Import and configure the converted IRO's via the same method [outlined in steps 5 & 6 above.](https://github.com/julianxhokaxhiu/FFNx#PNG-To-DDS-Conversion)



## Inspect logs
If you want to check what is going on behind the scene, or you may happen to have a crash, feel free to check the `FFNx.log` file, located in the same directory as `FFNx.dll`

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
  - for FF7Music which FFNx provides support for
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
See [LICENSE](LICENSE).

All rights belong to their respective owners.
