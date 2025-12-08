# Video Encoding Guide

This guide explains how to edit and encode FMV videos for high quality and proper playback within FFNx. For the most part, FFNx behaves like a generic ffmpeg-based video player. However, some default behavior differs in order to correctly play back Playstation 1 videos, upscales of Playstation 1 videos, FF7 PC release videos, and FF8 Steam release videos. **Therefore, videos of other provenance that are missing colorimetry metadata may play back with incorrect colors.** Additionally, FFNx does not handle the full range of exotic inputs that a media player like VLC might.

1. [Container Format](#container-format)
2. [Video Codec](#video-codec)
3. [Audio Codec](#audio-codec)
4. [Suggested Workflow](#suggested-workflow)
5. [Pixel Format](#pixel-format)
6. [Bit Depth](#bit-depth)
7. [Color Matrix](#color-matrix)
8. [Color Primaries (Color Gamut)](#color-primaries-color-gamut)
9. [Color Range](#color-range)
10. [Transfer Characteristics (Gamma)](#transfer-characteristics-gamma)
11. [Full Colorimetry Conversion Example](#full-colorimetry-conversion-example)
11. [Editing Metadata without Re-Encoding](#editing-metadata-without-re-encoding)

### Container Format
**Recommended:** mkv  
**Permitted:** Anything ffmpeg can decode  
**Notes:**
- Simply rename your file to end with .avi, regardless of its actual type.

### Video Codec
**Recommended:** x264  
**Permitted:** Anything ffmpeg can decode  
**Notes:**  
- x264 is recommended because it is widely used and well tested.
- The standalone x264 command-line encoder is recommended over ffmpeg or GUI-based tools.

### Audio Codec
**Recommended:** opus  
**Permitted:** Anything ffmpeg can decode  
**Notes:**  
- The audio for the Playstation 1 videos has an odd sample rate. It is recommended to resample to 48kHz using soxr before encoding. E.g.: `ffmpeg -i input.wav -af aresample=resampler=soxr -ar 48000 output.wav` (It's better to do a high-quality upsample at encode time and potentially downsample at playback time than to upsample at playback time.) 
- Since the additional space required is trivial relative to a typical FF7 + 7th Heaven install, encoding audio at a bitrate comfortably above "transparency" is recommended. (Consider 256 for opus, 320 for aac, `-q 9` for vorbis.) 
- If encoding to opus, the standalone opusenc encoder is recommended.
- If encoding to aac, it is strongly recommended to use Apple's encoder (see [qaac](https://github.com/nu774/qaac/wiki)) because ffmpeg's aac encoders are terrible.
- Be careful with vorbis encoders. Some will downsample to 22050Hz by default. This is undesirable because it distorts frequencies above [11025Hz](https://www.rapidtables.com/tools/tone-generator.html?f=11025), which are not only audible to humans, but also musically relevant. (See [Nyquist-Shannon theorem](https://en.wikipedia.org/wiki/Nyquist%E2%80%93Shannon_sampling_theorem).)


### Suggested Workflow
- Extract videos from Playstation disks to a series of .png images using [jPSXdec](https://github.com/m35/jpsxdec).
     - Also extract to any avi format to get wav audio. (Must be demuxed later.) 
- Do pre-processing with a frameserver and save output back to a series of .png images.
     - VapourSynth is recommended over AviSynth(+) because it has better performance, better stability, and does not have functions with undesirable "gotcha" default parameters like AviSynth does (e.g., ConvertToYUV() does a PC->TV range conversion by default).
     - [fmtconv](https://github.com/EleonoreMizo/fmtconv) is recommended for bit-depth, and pixel format conversions, for both VapourSynth and AviSynth.
     - There is no good answer for color conversion. fmtconv is not good for our use cases. (See below.) Frames could be saved as a series of .png images and then fed throught gamutthingy, but that's very time consuming. (Perhaps one day gamutthingy will be able to create LUTs in the format used by the Avisynth/Vapoursynth Timecube filter.) The best answer may be to avoid color conversion and depend on FFNx to convert at playback.
- Batch process your .png images through your AI upscaler program.
- Do post-processing with a frameserver, piping output to your encoder.
- Encode with the standalone x264 command-line encoder.
- Mux audio and video using mkvtoolsnix.
- Rename mkv file to avi.

### Pixel Format
**Recommended:** yuv420 family (yuv420p10le in specific)   
**Permitted:** Anything ffmpeg can decode  
**Notes:**  
- yuv420 is standard for digital video because the human vision has about half the resolving power for chroma as for luma.
- FFNx resamples to yuv444 internally. So, if you use yuv422 or yuv444, the extra chroma data won't be "wasted."
     - (Exception: BGR24 is not converted to yuv444; rather the RGB values are used directly. This pixel format is used by the avi files in the PC release of FF7.) 

### Bit Depth
**Recommended:** 10 bits per color  
**Permitted:** Anything ffmpeg can decode  
**Notes:**  
- FFNx converts to 8 bits per color internally, as it must for display on standard 8-bit monitors.
- Nevertheless, 10-bit encoding is recommended, even for 8-bit source material destined for an 8-bit monitor, because it reduces banding and compression artifacts and yields better quality at the same file size (or smaller file size at the same quality).
- Generally, the first thing your frameserver script should do is increase the bit depth to at least 16, and the last thing it should do is reduce the bit depth to 10. Working in a high bit depth prevents banding and other color errors caused by rounding.
- Depending on your operating system and the age of your x264 encoder, you might have separate binaries for 8-bit and 10-bit encoding, or a single binary that can do both. The relevant parameters for the dual-depth binary are `--input-depth` and `--output-depth` both of which should be 10. 

### Color Matrix
**Background Info:** The color matrix is used to convert between YUV and RGB. The same matrix must be used for playback as was used at encoding time.  
**x264 encoder parameter:** `--colormatrix` Possible values are `undef`, `bt709`, `fcc`, `bt470bg`, `smpte170m`, `smpte240m`, `GBR`, `YCgCo`, `bt2020nc`, `bt2020c`, `smpte2085`, `chroma-derived-nc`, `chroma-derived-c`, and `ICtCp`. This parameter only sets the metadata used for playback; it does not do a color matrix conversion.
**Recommended:** `bt709` for new material or if doing a color matrix and gamut conversion in a frameserver. Otherwise retain the source material's matrix. **For upscales/edits of Playstation videos, retain bt601 and leave the metadata absent.**
**Permitted:** Anything ffmpeg can decode  
**FFNx default behavior:** If the color matrix metadata is absent or `undef`, `smpte170m` (bt601) is assumed. Except `bt709` is assumed for FF8 when the video has HD dimensions and no colorimetry metadata.  
**Notes:**
- The original Playstation 1 videos use the bt601 color matrix. (See [jpsxdec documentation](https://github.com/m35/jpsxdec/blob/readme/jpsxdec/PlayStation1_STR_format.txt)).
     - There are some small errors in the Playstation 1's matrix values (rounding errors? electrical engineering compromises?), but jpsxdec accounts for them, so a standard matrix should be used thereafter.
- The avi video files from the PC edition of FF7 use an RGB pixel format, and thus no YUV->RGB color matrix is needed.
- The avi video files from the Steam edition of FF8 appear to use the bt709 color matrix.
- FFNx handles bt601 and bt709 natively. Anything else will be converted to bt601 using ffmpeg's swscale component. In this case, the metadata for color matrix, color primaries, and transfer properties must be set correctly. Otherwise ffmpeg must guess about these values when converting, and may guess incorrectly.
- The color matrix of an unlabeled video is almost always either bt601 or bt709. Which one can usually be ascertained by "eyeballing" it using a tool that can swap which matrix is used for display (such as [vsedit](https://github.com/YomikoR/VapourSynth-Editor)). [This page](http://avisynth.nl/index.php/Colorimetry) explains how greens and reds will look wrong when the incorrect matrix is used. For live-action content, you can usually tell by looking at a tree or bush. For animated and computer-generated content it can be more difficult to distinguish incorrect color from artistic intent.

### Color Primaries (Color Gamut)
**Background Info:** The color gamut is the range of visible light that a playback device is physically capable of producing. Video is "mastered" with a particular class of playback device in mind (e.g., American CRT television sets with SMPTE-C phospors). To correctly play back video mastered for one gamut on a device with a different gamut, a conversion must be made so that the second device will produce the same visible colors as the first, to the extent it's physically able to.  
**x264 encoder parameter:** `--colorprim` Possible values are `undef`, `bt709`, `bt470m`, `bt470bg`, `smpte170m`, `smpte240m`, `film`, `bt2020`, `smpte428`, `smpte431`, `smpte432`. This parameter only sets the metadata used for playback; it does not do a color gamut conversion.  
**Recommended:** `undef` for NTSC-J material, otherwise the color gamut of the source material (likely `bt709` when color matrix is bt709 and `smpte170m` or `bt470bg` when color matrix is bt601).   
**Permitted:** `undef`, `bt709`, `bt470m`, `smpte170m`, `smpte240m`, `bt470bg` _If video metadata specifies unsupported color primaries, the video will fail to play._  
**FFNx default behavior:** If the color primary metadata is absent or `undef`, the NTSC-J color gamut is generally assumed. Except that `bt709` is assumed when (1) the color matrix is bt709, or (2) for FF8 when the video has HD dimensions and no colorimetry metadata, or (3) if the file name is eidoslogo.avi or sqlogo.avi.  
**Notes:**
- The original Playstation 1 videos were mastered for playback using the color gamut of the P22 phosphors used in 1990s televisions, coupled with the "color correction" built in to television receivers to (crudely) compensate for the discrepancy between the phosphors and the Japanese broadcast specification.
     - Unfortunately, neither the x264 encoder nor ffmpeg have an enum for this color gamut. So FFNx must hijack `undef` to mean "NTSC-J." **Therefore, unless the exceptions noted above apply, a video that is not NTSC-J will play back with (very) incorrect colors in FFNx if the `--colorprim` metadata is absent.**
     - **Do not** attempt gamut conversions to/from NTSC-J unless you are 110% sure you know what you're doing. FFNx's gamut conversion is now **very** good, probably much better than whatever Avisynth filter you've found. So it's almost certainly a better idea to leave the color gamut alone and let FFNx deal with it. (Even if you are 110% sure you know what you're doing, it's probably wise to ask ChthonVII to be absolutely certain.)
          - Do not use fmtconv for conversions to/from NTSC-J color gamut. Its NTSC-J preset uses a somewhat wrong whitepoint, uses the unreachable color primary coordinates from the NTSC spec (FFNx uses realistic coordinates for P22 phosphors, and simulates the "color correction" that would have crudely compensated for the discrepancy from the spec), and does not use a gamut compression algorithm to handle colors outside the destination gamut without clipping.
          - [gamutthingy](https://github.com/ChthonVII/gamutthingy) can do the conversion correctly, but you would have to batch process one frame at a time in .png format. Perhaps one day gamutthingy will be able to create LUTs in the format used by the Avisynth/Vapoursynth Timecube filter.
- In the absence of metadata, `bt709` (sRGB) is assumed for eidoslogo.avi and sqlogo.avi because these files don't exist on the Playstation discs, and were presumably created for the PC release, presumably using the sRGB gamut. (It's honestly hard to say what the intended colorimetry for these two videos was, and sRGB is just the best guess available. We don't really know what was sitting on the desks of the people who made these videos. In 1998, the sRGB standard had been proposed, but not yet accepted. The first generation of consumer LCD monitors became available in 1997 and 1998, and *most* of them would have been at least close to sRGB. The colorimetry of CRT computer monitors was a "wild west.")
- The avi video files from the Steam edition of FF8 appear to use the bt709 color gamut. (It appears that gamut conversion from NTSC-J was already done when these videos were encoded.)
- The `bt709` color gamut is the same as sRGB, used by modern computer monitors. FFNx will ultimately convert everything to this color gamut. (Except if HDR is enabled, everything will instead be converted to the rec2020 color gamut used by HDR monitors.)
- Due to some compromises necessary for rendering gameplay, videos are subject to two sequential color gamut conversions in some circumstances. This is not ideal, but it cannot be avoided. Sequential color gamut conversions happen when:
     - When NTSC-J mode is enabled, and the video's color gamut is not NTSC-J.
     - When HDR is enabled, and NTSC-J mode is disabled, and the video's color gamut is not `bt709` (sRGB).
- The `bt470m` color gamut was deprecated in television standards in 1994. If `bt470m` metadata is encountered, FFNx will assume it's an error and that `smpte170m` was intended.
- `smpte170m` is SMPTE-C, the North American standard for standard definition television. `smpte240m` is identical.
- `bt470bg` is EBU (PAL), the European standard for standard definition television.
- The color primaries of an unlabeled video likely match its color matrix, as follows: `bt709` for bt709, `smpte170m` for NTSC bt601, `bt470bg` for PAL bt601. NTSC-J is limited to material mastered for playback on Japanese CRT television sets. Videos created from still images created in image editing software like Photoshop and GIMP likely use the sRGB (`bt709`) gamut.

### Color Range
**Background Info:** Whether or not the full bit depth is used. If not, color values must be shifted and scaled for playback. See first note below for details.  
**x264 encoder parameters:** `--input-range` and `--range` Possible values are `pc`, `tv`, and `auto`. The latter parameter sets the metadata used for playback. If the former parameter differs, the encoder will perform a conversion. However, converting in the framesever is preferrable.  
**Recommended:** `pc` (full range)   
**Permitted:** `pc`, `tv`, unspecified  
**FFNx default behavior:** If the color range metadata is absent, tv range is assumed. (Except for pixel formats that are inherently full range.)  
**Notes:**  
- Full range (PC range) uses the entire bit depth available. (0-255 for 8 bits per color.) TV range is compressed and shifted to avoid using the upper and lower ~9% of the bit depth. (16-235 luma and 16-240 chroma for 8 bits per color.) TV range was devised as a workaround for the unavoidable over- and undershoots in analog singal processing. (See [Gibbs phenomenon](https://en.wikipedia.org/wiki/Gibbs_phenomenon).) TV range persisted into the age of digital video for purposes of backwards compatibility with analog television sets. It serves little purpose today, and none whatsoever with respect to content destined for a computer monitor.  
- Conversion from full range to tv range should be avoided because color data is irreparably discarded. This tends to cause [color banding](https://en.wikipedia.org/wiki/Colour_banding).
- When playing back tv-range videos, FFNx shifts and scales color values up to full range, with dithering.
     - (Dithering is not performed on the DirectX 9 experimental backend because a necessary shader function isn't supported.)
- For a limited class of high-bit-depth, tv-range videos, it's theoretically possible to recover 8 bits of full-range data. However, FFNx does not attempt this because doing so would mean giving up dithering for _all_ high-bit-depth, tv-range videos. (In particular, the extremely popular SYW upscale videos are high bit depth and tv range, but aren't in this limited class, and therefore benefit only from dithering.)
- The original Playstation 1 videos are full range.
- The avi video files from the PC edition of FF7 are full range.
- The avi video files from the Steam edition of FF8 are tv range. (Surprising and disappointing.)
- Several ffmpeg-based video players (notably including VLC) suffer from a bug in which, under certain circumstances, they apply an unnecessary and incorrect tv->pc range conversion to video that is already full range. FFNx does **_NOT_** have this bug.
- The color range of an unlabeled video can be ascertained using the [histogram filter for VapourSynth](https://vsdb.top/plugins/hist) or [AviSynth](http://avisynth.nl/index.php/Histogram). (See the AviSynth link for useful documentation.)

### Transfer Characteristics (Gamma)
**Background Info:** The gamma function used to convert between linear RGB and gamma-encoded RGB. (See [gamma correction](https://en.wikipedia.org/wiki/Gamma_correction).) At playback time, the inverse of whatever gamma function was previously used must be used to recover linear RGB, which can then be re-gamma-encoded using the inverse of the playback device's gamma function.  
**x264 encoder parameter:** `--transfer` Possible values are `undef`, `bt709`, `bt470m`, `bt470bg`, `smpte170m`, `smpte240m`, `linear`, `log100`, `log316`, `iec61966-2-4`, `bt1361e`, `iec61966-2-1`, `bt2020-10`, `bt2020-12`, `smpte2084`, `smpte428`, and `arib-std-b67`. This parameter only sets the metadata used for playback; it does not do a gamma conversion.  
**Recommended:** The inverse of whatever gamma function was previously used to convert from linear RGB to gamma-encoded RGB. If you didn't do such a conversion, then retain the source material's transfer characteristics. (Also, **don't** do such a conversion on Playstation videos unless you have software that can perform the BT1886 Appendix 1 function. See gamma_and_gamut.md for details.)
**Permitted:** `undef`, `bt709`, `bt470bg`, `iec61966-2-1`, `smpte170m`, `bt2020-10`, `bt2020-12`, `iec61966-2-4`, `bt1361e` (ffmpeg's AVCOL_TRC_GAMMA22 is also supported, but there's no way to flag that in x264's metadata.)  _If video metadata specifies unsupported transfer characteristics, the video will fail to play._  
**FFNx default behavior:** If the transfer characteristics metadata is absent or `undef`, FFNx will generally use the BT1886 Appendix 1 EOTF function. Except `bt709` is assumed when the color matrix is bt709 or for FF8 when the video has HD dimensions and no colorimetry metadata, and `bt470bg` is assumed when the color gamut is bt470bg.
**Notes:**
- In the absence of metadata, FFNx assumes original Playstation 1 videos (or derivatives thereof). Since these videos were meant to play on a CRT television, FFNx uses the EOTF (gamma) function from BT1886 Appendix 1, with constants selected to match a mid-90s Sony Trinitron CRT with the brightness and contrast knobs turned to where it looks "good."
- `iec61966-2-1` is the sRGB gamma function. Modern sRGB computer monitors use this gamma function. FFNx will ultimately convert everything to this. The average of this function is roughly equivalent to a pure 2.2 curve. (Except if HDR is enabled, everything will instead be converted to the rec2084 ("PQ") gamma function used by HDR monitors.)
- `smpte170m`, `bt709`, `bt2020-10`, `bt2020-12`, `iec61966-2-4`, and `bt1361e` are all functionally identical. This is the gamma function used in the digital NTSC and HD television standards. The average of this function is roughly equivalent to a pure 1.9 curve (though 2.0 is often cited). This is deliberately overbright in order to counteract other factors with playback on CRT television sets.
- `bt470bg` is the gamma function for PAL standard definition television. It's a pure 2.8 gamma curve. (Despite the spec, physics dictated that PAL CRT televisions had the same gamma characteristics as other CRTs, so the BT1886 Appendix 1 EOTF functions captures them well. 2.8 should only be used for digital sources, if at all.)
-  Aside from Playstation 1 videos (see above), the transfer characteristics of an unlabeled video are most likely `smpte170m`.

### Full Colorimetry Conversion Example
(**WARNING:** This is ill-advised. Again, you probably should not mess with NTSC-J video. fmtconv's conversion is not very good. You should probably leave the conversion to FFNx instead.)
The correct frameserver procedure for completely converting all colorimetry properties is somewhat complicated. Therefore, an example is provided here. The steps are:
1. Increase working bit depth to (at least) 16 bits per color. If the input is tv range, do a range conversion in the same operation.
2. Convert to (gamma encoded) RGB using the source color matrix.
3. Convert to linear RGB using the source gamma function.
4. Convert from the source color gamut to the destination color gamut.
5. Convert to gamma encoded RGB using the destination gamma function.
    - However, if you want to use BT1886 Appendix 1 at playback, then instead just use the inverse of step 3 and set the `--transfer` metadata to `undef`.
6. Convert to YUV using the destination color matrix.
7. Downsample to 10 bits per color for output.
8. Set metadata encoder parameters correctly for the destination color properties.

Here is a sample VapourSynth script converting a Playstation video to bt709:
```
import vapoursynth as vs
core = vs.core
video = core.ffms2.Source('path/to/input.avi')
video = core.fmtc.bitdepth(video, bits=16, fulls=True, fulld=True) #use fulls=False if the source is tv range
video = core.std.SetFrameProp(video, prop='_ColorRange', intval=0) # 0 means full range
video = core.fmtc.resample(video,css='444', fulls=True, fulld=True) # YUV->RGB conversion will require 444
video = core.fmtc.matrix(video, mats="601", matd="RGB", fulls=True, fulld=True)
# assume 2.2 pure gamma curve for playstation videos; would normally use transs/transd instead of gcor, but there's no 2.2 option
video = core.fmtc.transfer(video, transs="linear", transd="linear", gcor=2.2, fulls=True, fulld=True)
video = core.fmtc.primaries(video, prims="ntscj", primd="709")
# Because we want to use FFNx's custom gamma function, rather than converting to bt709's gamma function, invert the earlier step and set metadata to undef
# Again, would normally use transs/transd instead of gcor, but there's no 2.2 option
video = core.fmtc.transfer(video, transs="linear", transd="linear", gcor=1.0/2.2, fulls=True, fulld=True)
# If we wanted to convert the gamma function to bt709, we'd use this instead
# video = core.fmtc.transfer(video, transs="linear", transd="bt709", fulls=True, fulld=True)
video = core.fmtc.matrix(video, mats="RGB", matd="709", fulls=True, fulld=True)
video = core.fmtc.resample(video,css='420', fulls=True, fulld=True)
video = core.fmtc.bitdepth(video, bits=10, fulls=True, fulld=False)
video.set_output()
# remember to set encoder parameters --input depth 10 --output-depth 10 --colormatrix bt709 --colorprim 709 --input-range pc --range pc --transfer undef (so that FFNx will use its custom gamma function)
```

### Editing Metadata without Re-Encoding
Because FFNx often interprets missing or undefined metadata in unusual ways in order to correctly play back its most common inputs (Playstation 1 videos, upscales of Playstation 1 videos, FF7 PC release videos, and FF8 Steam release videos), there will be circumstances where videos of other provenance that are missing colorimetry metadata will play back with incorrect colors. Such videos can be made to play back correctly in FFNx by editing their metadata to correctly describe their colorimetry.

It is possible to edit the metadata of a video stream using ffmpeg without re-encoding it. Please consult [ffmpeg's bitstream filters documentation](https://ffmpeg.org/ffmpeg-bitstream-filters.html). Assuming your video is x264, you will also need the tables in Annex E of [the h264 spec](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-H.264-201602-S!!PDF-E&type=items).

Note that ffmpeg is confused by renaming other containers (e.g., mkv) to avi, so the input file name should be changed back to the correct extension first.

Here is an example command that changes the video metadata to bt601 color matrix, undefined color primaries, undefined transfer characteristics, and full range:
```
ffmpeg -i input.mkv -c:v copy -bsf:v h264_metadata=matrix_coefficients=6:colour_primaries=2:transfer_characteristics=2:video_full_range_flag=1 -c:a copy output.mkv
```
