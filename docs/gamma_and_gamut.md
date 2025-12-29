# Gamma and Color Gamut

This file documents the gamma and color gamut behavior of FFNx.

Secondarily, it can also be used as a guide if you wish to make adjustments to this behavior.
(Be aware, though, that changing this behavior will require recompiling FFNx and/or its shaders.)

In a nutshell:
 - FFNx uses the EOTF function from BT1886 Appendix 1 as the gamma function in several situations. This function approximates the gamma behavior of a CRT television/computer monitor.
      - FFNx always uses the BT1886 Appendix 1 function for videos that don't specify a gamma function in their metadata, except for FF8 Steam videos.
      - If NTSC-J mode is enabled, FFNx uses the EOTF function from BT1886 Appendix 1 as the gamma function for *everything*, except videos that specify a gamma function in their metadata and FF8 Steam videos.
      - If NTSC-J mode is disabled, FFNx uses the sRGB gamma function generally, except for movies as explained above.
 - If NTSC-J mode is enabled, FFNx simulates the "color correction" behavior, phosphor color gamut, and whitepoint of a mid-90s Japanese-model professional-grade Sony Trinitron CRT television, then converts that to sRGB (or to rec2020 if HDR mode is enabled).
 - If NTSC-J mode is disabled, colors are displayed as-is, without any adjustments to account for the differences between 90s-era displays and modern displays.

Most of the necessary calculations are done using [gamutthingy](https://github.com/ChthonVII/gamutthingy).

## Why Sony Trinitron?

The choice of which particular CRT television to emulate -- a mid-90s Japanese-model professional-grade Sony Trinitron CRT television -- is based on the following:
- Most of the developers were working in Japan. The original target audience was Japan. These games were intended to be displayed on Japanese television sets.
- Trinitron dominated the television market during the mid 90s. It was common, and widely considered superior.
- Since FF7 and FF8 were developed exclusively for Playstation, it's plausible that Sony may have supplied some televisions to Square for development, or at least suggested that Square use their televisions. Or Square decided on their own to use Sony's televisions along with Sony's Playstation dev kit.
- Professional-grade televisions (PVMs) were used elsewhere in the game development world. (See this picture [note 1] of a developer's desk working on Pilotwings for SNES.) And Square's budget for FF7 was *gigantic*. They could afford them.
- Sony's PVMs had a redder red than their consumer models, giving them more "pop." We might as well emulate the best-looking possibility.

## Gamma

If NTSC-J mode is enabled, FFNx uses the EOTF function from BT1886 Appendix 1 [note 2] as the gamma function for everything, except videos specify a gamma function in their metadata and FF8 Steam videos (which are assumed to be BT709). Regardless of NTSC-J mode, FFNx always uses this EOTF function for videos tha tdon't specify a gamma function in their metadata, except for FF8 Steam videos  (Note that, quite confusingly, BT1886 contains both an "Annex 1" and an "Appendix 1." The Annex 1 function is pretty crummy. The Appendix 1 function is quite good.)

The parameters/constants for the BT1886 Appendix 1 function are found at the top of `misc/FFNx.common.sh`.
- `crtBlackLevel` times 100 is the black level of the CRT television/computer monitor in cd/m^2. (Corresponds to the "brightness" knob.)
- `crtWhiteLevel` times 100 is the white level of the CRT television/computer monitor in cd/m^2. (Corresponds to the "contrast" knob.)
- `crtConstantB`, `K`, `S`, and `I` are calculated from `crtBlackLevel` and `crtWhiteLevel`. (They must be recalculated if `crtBlackLevel` or `crtWhiteLevel` changes. And some LUTs would need to be recalculated too.)

The choice of `crtBlackLevel = 0.0018` and `crtWhiteLevel = 1.5` took a properly calibrated Trinitron as its starting point, then made adjustments until the results look good enough to plausibly be what the developers saw and approved.

A properly calibrated Trinitron probably had a range somewhere around 0.01 cd/m^2 to 171 cd/m^2. [notes 3 & 4] However, this doesn't look great for FF7/8, so this is probably not how developers' televisions were calibrated. (Note also that this black is 10x lower than what BT1886 calls a "moderate" black level, 0.1 cd/m^2. [note 2])

In making adjustments, the following were considered:
- The final black and white levels have to stay within a plausible distance of the "properly" calibrated values above (and BT1886's "moderate" black level).
- If the sRGB gamma function is used, the FF7 opening movie exhibits *horrible* banding in the dark sky around the stars. For the most part, that is avoided simply by virtue of the BT1886 Appendix 1 being somewhat non-linear near zero. However, we do want to be careful not to raise the black level too far, lest this banding might appear.
- We want to raise the black level (brightness) far enough that shadowy bits aren't totally lost to blackness, but not so far that they stop looking shadowy.
     - One touchstone for this is the wall screen-right of Aerith as she stands up during the FF7 opening movie.
          - If the black level is too low, the whole wall is lost to darkness. You can't see that it's brick up to knee level and has a window at head level.
          - If the black level is too high, you can see the lack of detail work in places that were supposed to be covered in shadow: The window has no detail work other than the frame; the panes are just the wall texture. And you can see where the wall stops and the rest of the alley is just black.
- Similarly, we want to raise the black level enough that shadowed parts of character faces and field maps usually aren't too dark to see well.
- We want to lower the white level (contrast) far enough that battle scenes with the beach background don't look like black figures on a white beach.

## Color Gamut Conversions Generally

FFNx does color gamut conversions in three situations:
 - If NTSC-J mode is enabled, *everything* must be converted from NTSC-J to sRGB (or to rec2020 if HDR mode is enabled).
 - If HDR mode is enabled, *everything* must be converted to rec2020, either from sRGB or NTSC-J, depending on whether NTSC-J mode is enabled.
 - If a movie's metadata says its gamut is something other than the gamut used for everything else (sRGB or NTSC-J, depending on whether NTSC-J mode is enabled) then it must be converted. (And it must be converted immediately, because both games draw things on top of movies, so we need to get the colorimetry matching before that happens.)

## NTSC-J to sRGB Gamut Conversion

This conversion is done using the LUT `misc/glut_ntscj_to_srgb.png` because it's not amenable to calculating in real-time. The following things are baked into this LUT:
- Simulation of a "color correction chip."
     - **Historical Background Info:** Why did CRT televisions have "color correction"? The Japanese NTSC-J standard is essentially the same as the 1953 American NTSC standard, except for a different whitepoint. The chromaticities for the NTSC color primaries (red, green, and blue) were *ambitiously* selected to match the color range of movie film. At the time, all three phosphors were very dim, and there wasn't a known phosphor that matched the green coordinates, but the drafters had faith that the march of technological progress would soon bring phosphors the glowed brightly at the chosen coordinates. It didn't. By the late 50s, substantially brighter phosphors had been formulated, but they were less saturated. Nevertheless, manufacturers and consumers preferred them. By the mid 60s, people were paying attention to the problems caused by the mismatch between the NTSC broadcast spec and the actual phosphor colors. In 1966, N.W. Parker published a paper [note 5] that explained an inexpensive method for crudely compensating for this mismatch that rolled the math into the axis and gain constants used for demodulation, thus requiring zero additional components (just different calibration of existing components). Parker's method was admittedly crude, but doing it right would have required microchips to do digital computation that would have been prohibitively expensive for consumer goods in 1966, if the computation could even have been done in real time at all. (Note that the microchip was invented in 1959.) Industry chemists *never* achieved bright phosphors at the NTSC coordinates. Eventually, the Americans gave up and changed their spec in 1994 to bring broadcasts into line with the phosphors in common use (although adoption of the new standard was very slow, and not so much completed as eventually abandoned in favor of HDTV). But the Japanese kept their spec -- and the color correction necessitated by the spec/phosphor mismatch -- until the end of the CRT era. Parker's method remained in use up until the end of the CRT era, likely because it was cheap.
     - The chip selected for emulation is the CXA2060BS [note 6] set to "Japan" mode. This chip was used in Triniton PVMs released ~1998 [note 7] and subjectively looks good with the phosphor selection noted below.
          - (The specific 1996 PVM model tested below used the earlier CXA1739S chip, but I cannot locate a data sheet for it. On the basis of subjectively visually pleasing results with the CXA2060BS, the color correction behavior of CXA1739S is assumed to be identical or very similar.)
     - The output from the color correction stage is not clamped, except for values so negative that they would drive the electron gun in reverse and suck light out of the room (which is impossible). This is the weakest point of the research into CRT behavior, and this conclusion might be limited to particular models, or even wrong altoghter. This conclusion is based on: (1) the fact that the electron guns generally *could* be run at higher or lower voltages than they were for "white" and "black," as evidenced by the calibration knobs' ability to change what "white" and "black" were; (2) the absence of materials describing such clamping; (3) the absence of noticeable red clipping in console video games; (4) one test of a 2000 Panasonic CRT that didn't observe red clipping [note 8]; and (5) the fact that the NES game The Immortal purposefully used a "blacker than black" color, and this *worked* on CRTs of the time. (The reason we lack for documentation about (potentially) clamping at this stage is probably because it did not matter for broadcasts. "Broadcast safe" colors were limited to 75% in order to keep the overall composite signal within valid range for broadcast modulation. The highest output from the color correction stage was typically pure red ~1.3x. And 0.75 * ~1.3 ~= 1.0. Whether there was clamping would only have mattered for things like video game consoles that didn't have to care about "broadcast safe" restrictions.)
- A small (4%) increase of the "saturation" knob, because it subjectively looks good, and helps character models (especially ninostyle replacement field models) to stand out from the background.
- Linearization using the BT1886 Appendix 1 EOTF function *with the same parameters as used elsewhere*. (See above.)
- Basic gamut conversion. Linear RGB to XYZ using the source gamut primaries, followed by XYZ to linear RGB using the destination gamut primaries.
     - The CIE 1931 chromaticity coordinates for the phosphors used in mid/late-90s Trinitron PVMs were probably around red x=0.63, y=0.345; green x=0.281, y=0.606; blue x=0.152, y=0.067. These red coordinates are taken from measurements done on a PVM model released in 1996. [note 9] The measured green and blue coordinates closely matched a statement given by Sony to the authors of [note 10] on the chromaticity of Trinitron computer monitors, so the exact green and blue coordinates from Sony's statement are used.
          - It's indeed plausible that the red phosphor, and only the red phosphor, was different between a 1996 PVM and ~1995 consumer units. The only notable advance in phosphor technology in the 90s was an improved red (by way of a filter that reduced reflection of ambient light) that Sony had a proof-of-concept for in 1994. [notes 11 and 12] It's plausible that this improved technology was available in the expensive PVM line relatively soon after, like the 1996 unit tested.
- Chromatic adaptation to "move" the source gamut's whitepoint to match the destination gamut's.
     - After converting linear RGB to XYZ with source primaries, add these steps: first convert XYZ to rho-gamma-beta cone cell response space, then apply a chromatic adapation matrix, then convert back to XYZ. Then proceed as normal converting XYZ to linear RGB with destination primaries.
     - Uses the "CAT16" chromatic adapation matrix from [note 13].
     - The whitepoint of a Japanese model Triniton was probably around CIE 1931 chromaticity coordinates x=0.28345, y=0.29775.
          - The NTSC-J specification called for a broadcast whitepoint at 9300K+8MPCD, and a receiver whitepoint at 9300K+27MPCD. (I have no idea what the rationale was for having different broadcast and receiver whitepoints.) However, some manufacturers used 9300K+8MPCD for recievers. It appears Sony was among them. [notes 4 and 14]
          - There are two sets of slightly different coordinates for 9300K+8MPCD because there are two slightly different units named "MPCD." [notes 15, 16, and 17] The average is used here. The difference is small enough that even a trained observer probably cannot detect it.
- Gamut compression to move colors outside the destination gamut in-bounds without clipping. Gamut compression is performed in the perceptually uniform JzCzhz color space. [note 18] Prior to gamut compression, the source gamut is pruned, and selective hue rotation is performed (Spiral CARISMA), both of which are explained in gamutthingy's readme file.  Gamut compression is done using the VPRC algorithm, a modification of the VP algorithm in [note 19]. The modifications are explain in gamutthingy's readme file. Gamut boundaries are determined using the method in [note 20].

The command used to generate `misc/glut_ntscj_to_srgb.png` was:
```
gamutthingy --lutgen true --lutsize 64 --lutmode normal --crtemu front --crtdemod CXA2060BS_JP --crtyuvconst 3digit --retroarchtextoutputfile infodump.txt --crtclamphighenable false --crtclamplowzerolight true --crtblack 0.0018 --crtwhite 1.5 --crt-saturation-knob 1.04 --source-primaries P22_trinitron_mixandmatch --source-whitepoint 9300K8mpcd --dest-primaries srgb_spec --dest-whitepoint D65 --adapt cat16 --spiral-carisma true --map-mode compress --gamut-mapping-algorithm vprc --gamma-out srgb --outfile glut_ntscj_to_srgb.png
```
The indices to this LUT are in NTSC-J CRT gamma-space R'G'B' without color correction. In order to calculate weights for interpolation the input and neighboring indices are fed through `CRTSimulation()` from `misc/FFNx.common.sh`. (See below.)

This LUT's values are stored using sRGB gamma, and must be linearized before interpolation. (The LUT is stored using sRGB gamma so that more bandwidth is dedicated to colors where human vision is more sensitive.)

## NTSC-J to rec2020 Gamut Conversion

All of the informational notes regarding "NTSC-J to sRGB" gamut conversion apply here too.

Unlike "NTSC-J to sRGB" gamut conversion, a LUT is not used here. Because the rec2020 gamut fully encompasses the Trinitron PVM phosphor gamut, gamut compression is not needed. The remaining math is lightweight enough to implement in FFNx's shaders. "NTSC-J to rec2020" conversion proceeds as follows:

First, `CRTSimulation()` from `misc/FFNx.common.sh` applies a matrix that rolls together simulation of the color control chip and the saturation knob, followed by the BT1886 Appendix 1 EOTF function. The matrix can be obtained by running the gamutthingy command for generating `misc/glut_ntscj_to_srgb.png` (above), in the console output under the heading "CRT matrix incorporating demodulation (color correction), hue knob, and saturation knob" or in the text file specified by `--retroarchtextoutputfile`. A slightly different version of of the BT1886 Appendix 1 function is used here that can process inputs >1.0, giving outputs >1.0.

Second, `convertGamut_NTSCJtoREC2020()` from `misc/FFNx.common.sh` applies a matrix that rolls together gamut conversion and chromatic adaptation. (Gamut compression is not needed.) This matrix can be obtained by running the gamutthingy command below, in the console output under "Overall linear RGB to linear RGB transformation matrix."

```
gamutthingy -c 0x123456 --source-primaries P22_trinitron_mixandmatch --source-whitepoint 9300K8mpcd --dest-primaries rec2020_spec --dest-whitepoint D65 --adapt cat16 --map-mode clip
```

Values >1.0 produced by the foregoing are acceptable in this situation because they typically oughtn't exceed 2.0 and `ApplyREC2084Curve()` will multiply them by around 0.02.

## sRGB to rec2020 Gamut Conversion

"sRGB to rec2020" is the only mercifully easy gamut conversion handled by FFNx. It requires no color correction simulation, no chromatic adapation, and no gamut compression. Just one easy matrix for basic gamut conversion applied by `convertGamut_SRGBtoREC2020()` from `misc/FFNx.common.sh`. This matrix can be obtained by running the gamutthingy command below, in the console output under "Overall linear RGB to linear RGB transformation matrix."

```
gamutthingy -c 0x123456  --source-primaries srgb_spec --source-whitepoint D65 --dest-primaries rec2020_spec --dest-whitepoint D65 --map-mode clip
```

## sRGB to NTSC-J Gamut Conversion

This conversion is only used in NTSC-J mode for movies with metadata that specifies BT709 (which is the same as sRGB, except the gamma function) or FF8 Steam movies, which are assumed to be BT709. This conversion is the inverse of the global "NTSC-J to sRGB" conversion that will be performed later. Together the two conversions constitute a round trip, displaying the video with BT709 colors. (We cannot simply shut off global "NTSC-J to sRGB" conversion while these movies are playing because the game engine sometimes draws things on top of movies that need that conversion. Instead we need to get the movies into the same colorspace as the things tht will be drawn over it.)

This conversion is done using the LUT `misc/glut_inverse_ntscj_to_srgb.png`. The command used to generate this LUT was:
```
gamutthingy --lutgen true --lutsize 64 --lutmode normal --crtemu front --crtdemod CXA2060BS_JP --crtyuvconst 3digit --retroarchtextoutputfile infodump.txt --crtclamphighenable false --crtclamplowzerolight true --crtblack 0.0018 --crtwhite 1.5 --crt-saturation-knob 1.04 --source-primaries P22_trinitron_mixandmatch --source-whitepoint 9300K8mpcd --dest-primaries srgb_spec --dest-whitepoint D65 --adapt cat16 --spiral-carisma true --map-mode compress --gamut-mapping-algorithm vprc --gamma-out linear --backwards true --outfile glut_inverse_ntscj_to_srgb.png
```
The indices to this LUT are in sRGB space linear RGB.

This LUT's values are stored in NTSC-J CRT gamma-space R'G'B' without color correction, and must be linearized before interpolation.

## SMPTE-C to NTSC-J Gamut Conversion

This conversion is only used in NTSC-J mode for movies with metadata that specifies SMPTE-C gamut. This conversion is the inverse of a "NTSC-J to SMPTE-C" conversion. Together with the global "NTSC-J to sRGB" conversion that will be performed later, the two conversions constitute a two-hop trip, ultimately converting SMPTE-C to sRGB. (We cannot use a simple one-hop conversion because the game engine sometimes draws things on top of movies that need that "NTSC-J to sRGB" conversion. Instead we need to get the movies into the same colorspace as the things tht will be drawn over it.)

This conversion is done using the LUT `misc/glut_inverse_ntscj_to_spmtec.png`. The command used to generate this LUT was:
```
gamutthingy --lutgen true --lutsize 64 --lutmode normal --crtemu front --crtdemod CXA2060BS_JP --crtyuvconst 3digit --retroarchtextoutputfile infodump.txt --crtclamphighenable false --crtclamplowzerolight true --crtblack 0.0018 --crtwhite 1.5 --crt-saturation-knob 1.04 --source-primaries P22_trinitron_mixandmatch --source-whitepoint 9300K8mpcd --dest-primaries smptec_spec --dest-whitepoint D65 --adapt cat16 --spiral-carisma true --map-mode compress --gamut-mapping-algorithm vprc --gamma-out linear --backwards true --outfile glut_inverse_ntscj_to_smptec.png
```

The indices to this LUT are in SPMTE-C space linear RGB.

This LUT's values are stored in NTSC-J CRT gamma-space R'G'B' without color correction, and must be linearized before interpolation.

## EBU to NTSC-J Gamut Conversion

This conversion is only used in NTSC-J mode for movies with metadata that specifies EBU gamut. This conversion is the inverse of a "NTSC-J to EBU" conversion. Together with the global "NTSC-J to sRGB" conversion that will be performed later, the two conversions constitute a two-hop trip, ultimately converting EBU to sRGB. (We cannot use a simple one-hop conversion because the game engine sometimes draws things on top of movies that need that "NTSC-J to sRGB" conversion. Instead we need to get the movies into the same colorspace as the things tht will be drawn over it.)

This conversion is done using the LUT `misc/glut_inverse_ntscj_to_ebu.png`. The command used to generate this LUT was:
```
gamutthingy --lutgen true --lutsize 64 --lutmode normal --crtemu front --crtdemod CXA2060BS_JP --crtyuvconst 3digit --retroarchtextoutputfile infodump.txt --crtclamphighenable false --crtclamplowzerolight true --crtblack 0.0018 --crtwhite 1.5 --crt-saturation-knob 1.04 --source-primaries P22_trinitron_mixandmatch --source-whitepoint 9300K8mpcd --dest-primaries ebu_spec --dest-whitepoint D65 --adapt cat16 --spiral-carisma true --map-mode compress --gamut-mapping-algorithm vprc --gamma-out linear --backwards true --outfile glut_inverse_ntscj_to_ebu.png
```

The indices to this LUT are in EBU space linear RGB.

This LUT's values are stored in NTSC-J CRT gamma-space R'G'B' without color correction, and must be linearized before interpolation.

## SMPTE-C to sRGB Gamut Conversion

This conversion is only used when NTSC-J mode is disabled, for movies with metadata that specifies SMPTE-C gamut. This is a "spec to spec" conversion, so no color correction simulation is required. However, gamut compression is required because SMPTE-C includes colors that are out-of-bounds for sRGB; and so a LUT is used.

This conversion is done using the LUT `misc/glut_smptec_to_srgb.png`. The command used to generate this LUT was:
```
gamutthingy --lutgen true --lutmode normal --lutsize 64 --source-primaries smptec_spec --source-whitepoint D65 --gamma-in linear --dest-primaries srgb_spec --dest-whitepoint D65 --spiral-carisma true --map-mode compress --gamut-mapping-algorithm vprc --gamma-out srgb --outfile glut_smptec_to_srgb.png
```

The indices to this LUT are in SMPTE-C space linear RGB.

This LUT's values are stored using sRGB gamma, and must be linearized before interpolation. (The LUT is stored using sRGB gamma so that more bandwidth is dedicated to colors where human vision is more sensitive.)

## EBU to sRGB Gamut Conversion

This conversion is only used when NTSC-J mode is disabled, for movies with metadata that specifies EBU gamut. This is a "spec to spec" conversion, so no color correction simulation is required. However, gamut compression is required because EBU includes colors that are out-of-bounds for sRGB; and so a LUT is used.

This conversion is done using the LUT `misc/glut_ebu_to_srgb.png`. The command used to generate this LUT was:
```
gamutthingy --lutgen true --lutmode normal --lutsize 64 --source-primaries ebu_spec --source-whitepoint D65 --gamma-in linear --dest-primaries srgb_spec --dest-whitepoint D65 --spiral-carisma true --map-mode compress --gamut-mapping-algorithm vprc --gamma-out srgb --outfile glut_ebu_to_srgb.png
```

The indices to this LUT are in EBU space linear RGB.

This LUT's values are stored using sRGB gamma, and must be linearized before interpolation. (The LUT is stored using sRGB gamma so that more bandwidth is dedicated to colors where human vision is more sensitive.)

## Linear RGB Colorspaces

FFNx's shader stack flips back and forth between gamma-space R'G'B' and linear RGB, doing some operations, most notably advanced lighting, in linear RGB. When NTSC-J mode is disabled, linear sRGB is used. When NTSC-J mode is enabled, "CRT linear RGB" is used. That is, color correction simulation followed by linearization with the EOTF function from BT1886 Appendix 1. This is accompished by `CRTSimulation()`, and reversed by `InverseCRTSimulation()`, both from `misc/FFNx.common.sh`. Because CRT color correction produced out-of-bounds values that weren't clamped (see above), operations in "CRT linear RGB" need to correctly deal with inputs >1.0.

## Footnotes

- [1] [Image](https://github-production-user-asset-6210df.s3.amazonaws.com/40120498/391341848-12925c41-58a7-4c79-8333-1341c9499133.png?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAVCODYLSA53PQK4ZA%2F20250802%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20250802T035224Z&X-Amz-Expires=300&X-Amz-Signature=67752f1fb98e4bc0d942f916f5a68aaaaa126915a442b4a25edfe234470a1574&X-Amz-SignedHeaders=host) from "SNES (Super Famicom) Software Development Kit (SDK)" at [retroreversing.com](https://www.retroreversing.com/super-famicom-snes-sdk/).
- [2] International Telecommunication Union. "Recommendation ITU-R BT.1886: Reference electro-optical transfer function for flat panel displays used in HDTV studio production." March, 2011. ([Link](https://www.itu.int/rec/R-REC-BT.1886-0-201103-I/en))
- [3] Soneira, Raymond. "Display Technology Shoot-Out: Comparing CRT, LCD, Plasma and DLP Displays." 2005(?) ([Link](https://www.displaymate.com/ShootOut_Part_1.htm))
- [4] Bohnsack, David L.; Diller, Lisa C.; Yeh, Tsaiyao; Jenness, James W.; and Troy, John B. "Characteristics of the Sony Multiscan 17se Trinitron color graphic display." *Spatial Vision*, Vol. 10, No. 4, pp. 345-51. 1997. ([Link](https://www.scholars.northwestern.edu/en/publications/characteristics-of-the-sony-multiscan-17se-trinitron-color-graphi))
- [5] Parker, Norman W. "An Analysis of the Necessary Decoder Corrections for Color Receiver Operation with Non-Standard Receiver Primaries." *IEEE Transactions on Broadcast and Television Receivers*, Vol. BTR-12, No. 1, pp. 23-32. April 1966 ([Link](https://ieeexplore.ieee.org/document/4319950)); republished in  *IEEE Transactions on Consumer Electronics*, Vol. CE-28, No. 1, pp. 74-83. February 1982. ([Link](https://ieeexplore.ieee.org/document/4179914))
- [6] CXA2060BS data sheet. ([Link](https://www.alldatasheet.com/datasheet-pdf/view/46749/SONY/CXA2060BS.html))
- [7] Service Manual for PVM 14N and 20N series. ([Link](https://consolemods.org/wiki/images/0/05/Sony_SSM_PVM-20N6U_20N5U_14N6U_14N5U_Service_Manual.pdf))
- [8] Impromptu test by Patchy68k ([Link](https://github.com/ChthonVII/gamutthingy/issues/1#issuecomment-2661096849))
- [9] Sony PVM 20M2U measured by Keith Raney ([Link](https://github.com/danmons/colour_matrix_adaptations/blob/main/csv/inputs.csv))
- [10] Has, Michael & Newman, Todd. "Color Management: Current Practice and The Adoption of a New Standard." *TAGA (Technical Association of Graphic Arts) Proceedings*, Vol. 2, pp. 748-771. 1995. ([Link](https://www.color.org/wpaper1.xalter))
- [11] Yen, William; Shionoya, Shigeo; and Yamamoto, Hajime. "Phosphor Handbook Second Edition," sections 6.2.3 and 18.6.2. CRC Press, Boca Raton (2007).
- [12] Ohno, Katsutoshi & Kusunoki, Tsuneo. "The Effect of Ultrafine Pigment Color Filters on Cathode Ray Tube Brightness, Contrast, and Color Purity." *Journal of the Electrochemical Society*, Vol. 143, No. 3, p. 1063 (1996). ([Link](https://iopscience.iop.org/article/10.1149/1.1836583))
- [13] Li, Changjun; Li, Zhiqiang; Wang, Zhifeng; Xu, Yang; Luo, Ming Ronnier; Cui, Guihua; Melgosa, Manuel; & Pointer, Michael. "A Revision of CIECAM02 and its CAT and UCS." *Proc. IS&T 24th Color and Imaging Conf.*, pp. 208-212 ([Link](https://library.imaging.org/admin/apis/public/api/ist/website/downloadArticle/cic/24/1/art00035))
- [14] Patchy68k. "Reverse the whitepoint from Sony CXA1464AS's datasheet." ([Link](https://github.com/ChthonVII/gamutthingy/issues/1))
- [15] gamutthingys' MPCD.md readme. ([Link](https://github.com/ChthonVII/gamutthingy/blob/master/MPCD.md))
- [16] Nagaoka, Yoshitomi. "テレビジョンの色再現と基準白色 (On the Color Reproduction and Reference White of Color Television)." *テレビジョン学会誌* (*Journal of the Television Society*), Vol. 33, No. 12, pp. 1013-1020. 1979. ([Link](https://www.jstage.jst.go.jp/article/itej1978/33/12/33_12_1013/_article/-char/ja/))
- [17] Yagishita, Shigeru; Nishino, Kenji; Ohta, Katsuhiro; & Ishii, Takashi. "カラーマスターモニター用基準白色内蔵カラーブラウン管 (Color Picture Tube with Built in Reference White for Color Master Monitors)." *テレビジョン* (*Television*), Vol. 31, No. 11, pp. 883-888. 1977. ([Link](https://www.jstage.jst.go.jp/article/itej1954/31/11/31_11_883/_article/-char/ja/))
- [18] Safdar, Muhammad; Cui, Guihua; Kim, You Jin; & Luo, Ming Ronnier. "Perceptually uniform color space for image signals including high dynamic range and wide gamut." *Optics Express*, Vol. 25, No. 13, pp. 15131-15151. June 2017. ([Link](https://opg.optica.org/fulltext.cfm?rwjcode=oe&uri=oe-25-13-15131&id=368272))
- [19] Xu, Liaho; Zhao, Baiyue; & Luo, Ming Ronnier. "Colour gamut mapping between small and large colour gamuts: Part I. gamut compression." *Optics Express*, Vol. 26, No. 9, pp. 11481-11495. April 2018. ([Link](https://opg.optica.org/oe/fulltext.cfm?uri=oe-26-9-11481&id=385750))
- [20] Xu, Lihao; Xu, Chunzhi; & Luo, Ming Ronnier. "Accurate gamut boundary descriptor for displays." *Optics Express*, Vol. 30, No. 2, pp. 1615-1626. January 2022. ([Link](https://opg.optica.org/fulltext.cfm?rwjcode=oe&uri=oe-30-2-1615&id=466694))
