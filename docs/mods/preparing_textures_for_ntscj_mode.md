# Preparing Textures for NTSC-J Mode

This guide addresses how to prepare textures to look correct in NTSC-J mode.

There are four general scenarios to consider:

## **1. The texture is an upscale/edit of an original texture, retaining the original colors.**

In this case, you do not need to do anything. (Unless you are very anal. See below.)

Just be aware that the colors you see in Photoshop, etc. are wrong. So do not "eyeball" color adjustments in Photoshop.

## **2. The texture is an original work, inspired by the original texture and hewing fairly close to the original colors.**

In this case, it's probably best to do nothing. If your asset is close to the original colors, as wrongly displayed in Phohoshop, etc., then it will also display close to the original colors, as correctly displayed in NTSC-J mode. (Even though you didn't take color correction into consideration, then end result will be about the same as if you had.)

(If you are very anal, see below.)

## **3. The texture is an original work, inspired by the original texture, but departing significantly from the original colors.**

Recolor your texture as per "If you are very anal," below.

## **3. The texture is an entirely original work, using colors you selected because they look good in sRGB.**

Make your texture in sRGB in Photoshop, etc., then apply the inverse NTSC-J conversion. Later, at display time, NTSC-J mode will apply the forward conversion, thus roundtripping your texture back to what you saw in Photoshop, etc.

To do the inverse conversion, use [gamutthingy](https://github.com/ChthonVII/gamutthingy), as so:
`gamutthingy --crtemu front --crtdemod CXA2060BS_JP --crtyuvconst 3digit --crtclamphighenable false --crtclamplowzerolight true --crtblack 0.0015 --crtwhite 1.71 --source-primaries P22_trinitron_mixandmatch --source-whitepoint 9300K8mpcd --dest-primaries srgb_spec --dest-whitepoint D65 --adapt cat16 --spiral-carisma true --map-mode compress --gamut-mapping-algorithm vprc --gamma-out srgb --backwards true --infile your_input_file.png --outfile your_output_file.png`

## **If you are very anal:**

If you are very anal, you can convert the original texture to sRGB, work on it (or use it for inspiration) in sRGB, then apply the inverse conversion to go back to NTSC-J colors.

This has two advantages:
1. If you eyeball colors in Photoshop, etc., what you see is what you get.
2. Color blending will be perfectly correct because what Photoshop, etc. thinks is the correct conversion to linear RGB will indeed be correct. (Though this hardly matters because color blending in the wrong linear space is still pretty close to correct. So the error is usually smaller than the error inherent in 8-bit quantization anyway.)

To forward convert from NTSC-J to sRGB:
`gamutthingy --crtemu front --crtdemod CXA2060BS_JP --crtyuvconst 3digit --crtclamphighenable false --crtclamplowzerolight true --crtblack 0.0015 --crtwhite 1.71 --source-primaries P22_trinitron_mixandmatch --source-whitepoint 9300K8mpcd --dest-primaries srgb_spec --dest-whitepoint D65 --adapt cat16 --spiral-carisma true --map-mode compress --gamut-mapping-algorithm vprc --gamma-out srgb --infile your_input_file.png --outfile your_output_file.png`

Make edits in sRGB space.

To inverse convert back to NTSC-J:
`gamutthingy --crtemu front --crtdemod CXA2060BS_JP --crtyuvconst 3digit --crtclamphighenable false --crtclamplowzerolight true --crtblack 0.0015 --crtwhite 1.71 --source-primaries P22_trinitron_mixandmatch --source-whitepoint 9300K8mpcd --dest-primaries srgb_spec --dest-whitepoint D65 --adapt cat16 --spiral-carisma true --map-mode compress --gamut-mapping-algorithm vprc --gamma-out srgb --backwards true --infile your_input_file.png --outfile your_output_file.png`
