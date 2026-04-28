# Video Encoding Guide

This guide explains how to encode FMV videos for high quality and proper playback within FFNx. In some respects, FFNx behaves like a generic ffmpeg-based video player such as VLC, mpv, etc. However, FFNx's default behavior in the absence of metadata is rather different because it's designed to play back the original game files. **Therefore, if you don't want your video treated like original game files, YOU MUST SET YOUR METADATA ACCORDINGLY.**

Also, FFNx's behavior differs between "NTSC-J mode" and "sRGB mode." NTSC-J mode honors typical metadata values for color primaries and transfer characteristics, but sRGB mode ignores them and treats all content as sRGB.

FFNx Behavior x Metadata Quick Reference Chart:
| Metadata Property | FFNx NTSC-J Mode Behavior | FFNx sRGB Mode Behavior |
|-------------------|---------------------------|-------------------------|
| Color Matrix, metadata set | Honors metadata. | Honors metadata. |
| Color Matrix, metadata undef | If FF7, bt601. If FF8, bink. | If FF7, bt601. If FF8, bink. |
| Color Primaries, metadata set | Honors metadata if `bt709`, `bt470m`, `smpte170m`, `smpte240m`; otherwise default behavior. (`bt470m` is presumed an error and treated as `smpte170m`.) | Ignores metadata. Always uses `bt709`. |
| Color Primaries, metadata undef | Simulation of CXA2060BS Japan mode color correction + Trinitron P22 phosphors. (Except no color correction for FF7 logo movies.) | `bt709` |
| Color Range, metadata set | Honors metadata. | Honors metadata. |
| Color Range, metadata undef | Limited (TV) range, unless pixel format is inherently full range. | Limited (TV) range, unless pixel format is inherently full range. |
| Transfer Characteristics, metadata set | Honors metadata if `bt709`, `iec61966-2-1`, `smpte170m`, `bt2020-10`, `bt2020-12`, `iec61966-2-4`, `bt1361e`; otherwise default behavior | Ignores metadata. Always uses `iec61966-2-1`. |
| Transfer Characteristics, metadata undef | bt1886 Appendix 1 EOTF function | `iec61966-2-1` |
| Chroma Sample Location, metadata set | Honors metadata. | Honors metadata. |
| Chroma Sample Location, metadata undef | Attempts guess based on codec and pixel format. Falls back to center. | Attempts guess based on codec and pixel format. Falls back to center. |


1. [Container Format](#container-format)
2. [Video Codec](#video-codec)
3. [Audio Codec](#audio-codec)
4. [Pixel Format](#pixel-format)
5. [Bit Depth](#bit-depth)
6. [Color Matrix](#color-matrix)
7. [Color Primaries (Color Gamut)](#color-primaries-color-gamut)
8. [Color Range](#color-range)
9. [Transfer Characteristics (Gamma)](#transfer-characteristics-gamma)
10. [Chroma Sample Location](#chroma_sample_location)
11. [Editing Metadata without Re-Encoding](#editing-metadata-without-re-encoding)

### Container Format
**Recommended:** mkv  
**Permitted:** Anything ffmpeg can decode  
**Notes:**
- Simply rename your file to end with .avi, regardless of its actual type.

### Video Codec
**Recommended:** h264  
**Permitted:** Anything ffmpeg can decode  
**Notes:**
- h264 is recommended because:
     - Its encoders and decoders are very mature and reliable.
     - Lower computational complexity relative to newer codecs helps (CPU) decode performance on old hardware.
     - Newer codecs are prone to blurring out fine details, whereas h264 is not. According to some commentators, h264 is the last codec truly capable of "transparent" encodes.
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

### Pixel Format
**Recommended:** yuv420 family (yuv420p10le in specific)  
**Permitted:** Anything ffmpeg can decode  
**Notes:**
- yuv420 is standard for digital video because the human vision has about half the resolving power for chroma as for luma.


### Bit Depth
**Recommended:** 10 bits per color  
**Permitted:** Anything ffmpeg can decode  
**Notes:**
- FFNx converts to 10 bits per color for internal processing.
- Most monitors (even many HDR monitors) accept only 8-bit input and require your GPU to do  a final conversion to 8-bit.
- Nevertheless, 10-bit encoding is recommended, even for 8-bit source material destined for an 8-bit monitor, because it reduces banding and compression artifacts and yields better quality at the same file size (or smaller file size at the same quality).
- Generally, the first thing your frameserver script should do is increase the bit depth to at least 16, and the last thing it should do is reduce the bit depth to 10. Working in a high bit depth prevents banding and other color errors caused by rounding.
- Depending on your operating system and the age of your x264 encoder, you might have separate binaries for 8-bit and 10-bit encoding, or a single binary that can do both. The relevant parameters for the dual-depth binary are `--input-depth` and `--output-depth` both of which should be 10.

### Color Matrix
**Background Info:** The color matrix is used to convert between Y'UV and R'G'B'. The same matrix must be used for playback as was used at encoding time.  
**x264 encoder parameter:** `--colormatrix` Possible values are `undef`, `bt709`, `fcc`, `bt470bg`, `smpte170m`, `smpte240m`, `GBR`, `YCgCo`, `bt2020nc`, `bt2020c`, `smpte2085`, `chroma-derived-nc`, `chroma-derived-c`, and `ICtCp`. This parameter only sets the metadata used for playback; it does not do a color matrix conversion.  
**Recommended:** `bt709` for new material or if doing conversions in a frameserver. Otherwise retain the source material's matrix.  
**Permitted:** Anything ffmpeg can decode  
**FFNx default behavior for FF7:** If the color matrix metadata is absent or `undef`, `smpte170m` (bt601) is assumed.  
**FFNx default behavior for FF8:** If the color matrix metadata is absent or `undef`, the matrix used by bink encoder/player circa v0.9f is assumed.  
**Notes:**
- The original Playstation 1 videos use the bt601 color matrix. (See [jpsxdec documentation](https://github.com/m35/jpsxdec/blob/readme/jpsxdec/PlayStation1_STR_format.txt)).
     - There are some small errors in the Playstation 1's matrix values (rounding errors? electrical engineering compromises?), but jpsxdec accounts for them, so a standard matrix should be used thereafter.
- The avi video files from the PC edition of FF7 use an R'G'B' pixel format, and thus no Y'UV->R'G'B' color matrix is needed.
- The video files from the both FF8's 2000 PC edition and Steam edition use the color matrix of the bink encoder/player circa v0.9f.
     - The 2000 PC edition videos are encoded as bink v0.9f. Except for the final credit roll, which is bink v0.9g.
     - The Steam edition videos are just simple upscales of the 2000 PC edition videos. The color matrix is unchanged.
     - The bink matrix is the result of bugs in the bink encoder/player transposing two pairs of constants in the standard bt601 calculations.
     - Accordingly, videos produced by the bink encoder look wildly wrong when played back using the standard bt601 matrix. And videos created with the standard bt601 matrix look wildly wrong when played back using the bink matrix. **So make sure to set (or deliberately leave unset) your color matrix metadata accordingly.**
     - A vapoursynth script to convert bink videos to R'G'B' (from which they can then be further converted to any standard matrix) is available at `/docs/mods/unbink.vpy`.
- FFNx handles bt601, bt709, and bink matrix natively. Anything else will be converted to bt601 using ffmpeg's swscale component.
- The color matrix of an unlabeled video is almost always either bt601 or bt709. Which one can usually be ascertained by "eyeballing" it using a tool that can swap which matrix is used for display (such as [vsedit](https://github.com/YomikoR/VapourSynth-Editor)). [This page](http://avisynth.nl/index.php/Colorimetry) explains how greens and reds will look wrong when the incorrect matrix is used. For live-action content, you can usually tell by looking at a tree or bush. For animated and computer-generated content it can be more difficult to distinguish incorrect color from artistic intent.

### Color Primaries (Color Gamut)
**Background Info:** The color gamut is the range of colors that a playback device is physically capable of producing. Video is "mastered" with a particular class of playback device in mind (e.g., American CRT television sets with SMPTE-C phosphors). To correctly play back video mastered for one gamut on a device with a different gamut, a conversion must be made so that the second device will produce the same colors as the first, to the extent it's physically able to.  
**x264 encoder parameter:** `--colorprim` Possible values are `undef`, `bt709`, `bt470m`, `bt470bg`, `smpte170m`, `smpte240m`, `film`, `bt2020`, `smpte428`, `smpte431`, `smpte432`. This parameter only sets the metadata used for playback; it does not do a color gamut conversion.  
**Recommended:** `undef` for NTSC-J material, otherwise the color gamut of the source material (likely `bt709` when color matrix is bt709 and `smpte170m` or `bt470bg` when color matrix is bt601).  
**Permitted, in NTSC-J mode:** `undef`, `bt709`, `bt470m`, `smpte170m`, `smpte240m`, If video metadata specifies unsupported color primaries, default behavior will be used.  
**FFNx default behavior, in NTSC-J mode:** If the color primary metadata is absent or `undef`, FFNx's NTSC-J mode will approximate the colors as they would have appeared a mid-90s Japanese-model professional-grade Sony Trinitron CRT television. See `/docs/color_modes.md` for more details. (Exception: If FF7, and the file name is `eidoslogo.avi` or `sqlogo.avi`, and the metadata for transfer function and color primaries are both blank, then the colors of a mid-90s Trinitron computer monitor will be approximated instead. Again see `/docs/color_modes.md` for more details.)  
**FFNx behavior, in sRGB mode:** *Ignores metadata.* Always uses `bt709`.  
**Notes:**
- The original Playstation 1 videos were mastered for playback using the color gamut of the P22 phosphors used in 1990s televisions, coupled with the "color correction" built in to television receivers to (crudely) compensate for the discrepancy between the phosphors and the Japanese broadcast specification.
- **Do not** attempt gamut conversions to/from NTSC-J unless you are 110% sure you know what you're doing. FFNx's gamut conversion is now **very** good, probably much better than whatever Avisynth filter you've found. So it's almost certainly a better idea to leave the color gamut alone, leave the metadata blank, and let FFNx deal with it.
     - Do not use fmtconv for conversions to/from NTSC-J color gamut. Its NTSC-J preset uses a somewhat wrong whitepoint, uses the color primary coordinates from the NTSC spec (rather "color correction" simulation, plus P22 phosphor coordinates), and does not use a gamut compression algorithm to handle colors outside the destination gamut without clipping.
     - [gamutthingy](https://github.com/ChthonVII/gamutthingy) can do the conversion correctly, but you would have to batch process one frame at a time in .png format. Perhaps one day gamutthingy will be able to create LUTs in the format used by the Avisynth/Vapoursynth Timecube filter.
- FF7's logo movies are a special case. Unlike the other movies that were mastered for playback via a PSX and television set, these movies were added for the PC98 release and presumably mastered for a mid-90s CRT computer monitor.
- The `bt470m` color gamut was deprecated in television standards in 1994. If `bt470m` metadata is encountered, FFNx will assume it's an error and that `smpte170m` was intended.
- `smpte170m` is SMPTE-C, the North American standard for standard definition television. `smpte240m` is identical.
- The color primaries of an unlabeled video likely match its color matrix, as follows: `bt709` for bt709, `smpte170m` for NTSC bt601, `bt470bg` for PAL bt601. NTSC-J is limited to material mastered for playback on Japanese CRT television sets. Videos created from still images created in image editing software like Photoshop and GIMP likely use the sRGB (`bt709`) gamut.

### Color Range
**Background Info:** Whether or not the full bit depth is used. If not, color values must be shifted and scaled for playback. See first note below for details.  
**x264 encoder parameters:** `--input-range` and `--range` Possible values are `pc`, `tv`, and `auto`. The latter parameter sets the metadata used for playback. If the former parameter differs, the encoder will perform a conversion. However, converting in the framesever is preferrable.  
**Recommended:** `pc` (full range)  
**Permitted:** `pc`, `tv`, unspecified  
**FFNx default behavior:** If the color range metadata is absent, tv range is assumed. (Except for pixel formats that are inherently full range.)  
**Notes:**
- Full range (PC range) uses the entire bit depth available. (0-255 for 8 bits per color.) TV range is compressed and shifted to avoid using the upper and lower ~9% of the bit depth. (16-235 luma and 16-240 chroma for 8 bits per color.) TV range was devised as a workaround for the unavoidable over- and undershoots in analog singal processing. (See [Gibbs phenomenon](https://en.wikipedia.org/wiki/Gibbs_phenomenon).) TV range persisted into the age of digital video for purposes of backwards compatibility with analog television sets. It serves little purpose today, and none whatsoever with respect to content destined for a computer monitor.
- When playing back tv-range videos, FFNx shifts and scales color values up to full range, with dithering.
- The original Playstation 1 videos are full range.
- The avi video files from the PC edition of FF7 are full range.
- The video files from the PC2000 and Steam editions of FF8 are (almost) tv range.
     - The bink encoder used for the videos in FF8's 2000 PC release is buggy and uses 16-234 for limited-range luma. (This affects the Steam FF8 videos too, because they are simple upscales of the PC2000 videos.) FFNx will correct for this if the color matrix metadata is left blank.
- Conversion from full range to tv range should be avoided because color data is irreparably discarded. This tends to cause [color banding](https://en.wikipedia.org/wiki/Colour_banding).
- Several ffmpeg-based video players (notably including VLC) suffer from a bug in which, under certain circumstances, they apply an unnecessary and incorrect range conversion. FFNx does **_NOT_** have this bug.
- The color range of an unlabeled video can be ascertained using the [histogram filter for VapourSynth](https://vsdb.top/plugins/hist) or [AviSynth](http://avisynth.nl/index.php/Histogram). (See the AviSynth link for useful documentation.)

### Transfer Characteristics (Gamma)
**Background Info:** The gamma function used to convert between linear RGB and gamma-encoded R'G'B'. (See [gamma correction](https://en.wikipedia.org/wiki/Gamma_correction).) At playback time, the inverse of whatever gamma function was previously used must be used to recover linear RGB, which can then be re-gamma-encoded using the inverse of the playback device's gamma function.  
**x264 encoder parameter:** `--transfer` Possible values are `undef`, `bt709`, `bt470m`, `bt470bg`, `smpte170m`, `smpte240m`, `linear`, `log100`, `log316`, `iec61966-2-4`, `bt1361e`, `iec61966-2-1`, `bt2020-10`, `bt2020-12`, `smpte2084`, `smpte428`, and `arib-std-b67`. This parameter only sets the metadata used for playback; it does not do a gamma conversion.  
**Recommended:** The inverse of whatever gamma function was previously used to convert from linear RGB to gamma-encoded R'G'B'. If you didn't do such a conversion, then retain the source material's transfer characteristics. (Also, **don't** do such a conversion on Playstation videos unless you have software that can perform the BT1886 Appendix 1 function. See `docs/color_modes.md` for details.)  
**Permitted, in NTSC-J mode:** `undef`, `bt709`, `iec61966-2-1`, `smpte170m`, `bt2020-10`, `bt2020-12`, `iec61966-2-4`, `bt1361e`. If video metadata specifies unsupported transfer characteristics, default behavior will be used.  
**FFNx default behavior, in NTSC-J mode:** If the transfer characteristics metadata is absent or `undef`, FFNx will use the BT1886 Appendix 1 EOTF function. See `docs/color_modes.md` for details.  
**FFNx behavior, in sRGB mode:** *Ignores metadata.* Always uses `iec61966-2-1`.  
**Notes:**
- For NTSC-J mode, in the absence of metadata, FFNx assumes original Playstation 1 videos (or derivatives thereof). Since these videos were meant to play on a CRT television, FFNx uses the EOTF (gamma) function from BT1886 Appendix 1, with constants selected to match a mid-90s Sony Trinitron CRT with the brightness and contrast knobs turned to where it looks "good."
- `iec61966-2-1` is the sRGB gamma function. Modern sRGB computer monitors use this gamma function. FFNx will ultimately convert everything to this. The average of this function is roughly equivalent to a pure 2.2 curve. (Except if HDR is enabled, everything will instead be converted to the rec2084 ("PQ") gamma function used by HDR monitors.)
- `smpte170m`, `bt709`, `bt2020-10`, `bt2020-12`, `iec61966-2-4`, and `bt1361e` are all functionally identical. This is the gamma function used in the digital NTSC and HD television/DVD/Bluray/etc. standards. The average of this function is roughly equivalent to a pure 1.9 curve (though 2.0 is often cited). This is deliberately overbright in order to counteract other factors with playback on CRT television sets.
-  Aside from Playstation 1 videos (see above), the transfer characteristics of an unlabeled video are most likely `smpte170m`.

### Chroma Sample Location
**Background Info:** Video is commonly encoded with a 420 pixel format (see above) in which the chroma planes are half the resolution of the luma plane. The chroma sample location states the position of the chroma samples relative to the corresponding 2x2 luma samples.  
**x264 encoder parameters:** `--chromaloc` Possible values are numbers 0-5 (0=left, 1=center, 2=topleft, 3=top, 4=bottomleft, 5=bottom; default 0). This parameter only sets the metadata used for playback; it does not do a conversion.  
**Recommended:** same as source  
**Permitted:** any  
**FFNx default behavior:** If the chroma sample location metadata is absent or `undef`, FFNx attempts to guess from codec default and pixel format. If unable to guess, falls back to center.  
**Notes:**
- As per the [jpsxdec documentation](https://github.com/m35/jpsxdec/blob/readme/jpsxdec/PlayStation1_STR_format.txt), the chroma sample location for Playstation 1 video is center.
- The chroma sample location for bink video is unknown.
- Be careful not to accidentally mess this up. Every conversion, in either direction, between a 420 pixel format and a 444 or R'G'B' pixel format involves chroma resampling. If you are not explicitly setting the chroma sample location for such conversions, then your tool is using some default, which may or may not match your source.

### Editing Metadata without Re-Encoding
Because FFNx often interprets missing or undefined metadata in unusual ways in order to correctly play back the original game files, there will be circumstances where videos of other provenance that are missing metadata will play back with incorrect colors. Such videos can be made to play back correctly in FFNx (at least in NTSC-J mode) by editing their metadata to correctly describe their colorimetry.

It is possible to edit the metadata of a video stream using ffmpeg without re-encoding it. Please consult [ffmpeg's bitstream filters documentation](https://ffmpeg.org/ffmpeg-bitstream-filters.html). Assuming your video is x264, you will also need the tables in Annex E of [the h264 spec](https://www.itu.int/rec/dologin_pub.asp?lang=e&id=T-REC-H.264-201602-S!!PDF-E&type=items).

Note that ffmpeg is confused by renaming other containers (e.g., mkv) to avi, so the input file name should be changed back to the correct extension first.

Here is an example command that changes the video metadata to bt601 color matrix, undefined color primaries, undefined transfer characteristics, and full range:
```
ffmpeg -i input.mkv -c:v copy -bsf:v h264_metadata=matrix_coefficients=6:colour_primaries=2:transfer_characteristics=2:video_full_range_flag=1 -c:a copy output.mkv
```
