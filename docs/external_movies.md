# External Movies

FFNx uses FFmpeg to read movie containers and audio. Video decoding is selected automatically: eligible movies use bgfx's native GPU decoder; other supported movies use FFmpeg's software decoder.

## Renderer Support on Windows

Select the renderer with `renderer_backend` in your FFNx configuration.

| Setting | Renderer | Movie decoding |
| --- | --- | --- |
| `0` | Auto | Depends on the selected renderer and its capabilities. |
| `1` | OpenGL | Software. |
| `3` | Direct3D11 | Hardware for eligible movies; software otherwise. |
| `4` | Direct3D12 | Hardware for eligible movies; software otherwise. |
| `5` | Vulkan | Hardware for eligible movies when the GPU and driver expose video decoding; software otherwise. |

Hardware decoding also requires GPU and driver support for the stream. A renderer supporting video decoding does not mean every codec, profile, level, or resolution is supported. The old `hardware_video_decoding` option is no longer used.

## Hardware-Decodable Movies

FFNx currently enables hardware decoding only for H.264/AVC streams meeting **all** of these requirements:

The bundled bgfx API exposes H.264, HEVC/H.265, and AV1 codec capabilities, but its current Direct3D11, Direct3D12, and Vulkan decoder implementations only initialize H.264 streams. HEVC and AV1 therefore remain software-decoded; reported GPU codec capabilities alone are not enough to enable them.

| Property | Required value |
| --- | --- |
| Video codec | H.264/AVC. |
| Pixel format | 8-bit planar YUV 4:2:0 (`yuv420p`). |
| Color matrix metadata | BT.709 (`bt709`). |
| Color range metadata | Limited/TV range (`tv`, also called MPEG range). |
| Chroma location metadata | Center or left. |
| Field order | Progressive or unknown; explicitly interlaced streams are not eligible. |
| Dimensions | Positive dimensions within the renderer's texture and video-decoder limits, including padding to multiples of 16. |
| Codec headers | Usable H.264 parameter sets that FFmpeg can prepare for the native decoder. |

These checks use the stream information available when the movie is opened. Missing color matrix, range, or chroma-location metadata does not satisfy the hardware requirements, even if software playback can infer suitable defaults.

Color matrix, color primaries, and transfer characteristics are separate properties. The BT.709 **matrix** requirement does not require BT.709 primaries or transfer characteristics; FFNx handles supported gamma and gamut metadata separately in its shaders.

### Chroma Alignment

The native decoder's YUV-to-RGB conversion uses centered chroma sampling. Center-sited movies match that sampling. Left-sited movies are accepted, but use the same centered sampling, producing a half-source-pixel chroma-alignment difference that may be visible at sharp colored edges. FFNx logs this approximation when applicable.

Software playback applies FFNx's chroma-location correction. For native decoding without the left-sited approximation, encode genuinely center-sited chroma; changing the metadata alone does not move the chroma samples.

## Software-Decoded Movies

The following use software decoding, provided the bundled FFmpeg build supports the codec and container:

- Codecs other than H.264, including HEVC/H.265, AV1, VP9, MPEG-2, MPEG-4 Part 2, MJPEG, and Bink.
- H.264 with 10-bit or higher samples, 4:2:2 or 4:4:4 chroma, or a pixel format other than `yuv420p`.
- H.264 with full-range color, a matrix other than BT.709, unsupported chroma-location metadata, or explicitly interlaced field order.
- Streams missing metadata required by the hardware checks, or without usable native-decoder headers.
- Otherwise eligible movies when the selected renderer, GPU, or driver lacks the required decoding capabilities.

Software decoding runs on the CPU; FFNx still uses GPU shaders to render the decoded frames and apply supported color processing. Playback follows the audio clock, or a wall clock for silent movies. When playback cannot present every source frame in time, stale presentation frames can be skipped while decoding reference frames in order.

Software fallback does not make an unsupported, corrupt, or oversized movie playable. High-resolution or high-frame-rate movies can also exceed the available CPU or rendering throughput.

## Containers and File Extensions

AVI, MP4, and MKV are containers, not video codecs. An AVI file is not necessarily software-decoded, and an MP4 file is not necessarily hardware-decoded: the contained video stream and its metadata determine eligibility.

FFmpeg identifies the actual container independently of the filename extension. Keep the movie basename expected by the game and use the extension selected by `ffmpeg_video_ext` (default: `avi`). For example, a supported MP4 or MKV container may be renamed to the game's expected `.avi` filename without changing its contents.

See the [Video Encoding Guide](mods/video_encoding_guide.md) for encoding and metadata guidance. Its 10-bit recommendation favors quality and reduced banding; **10-bit movies currently use software decoding**, whereas native hardware decoding requires 8-bit video. Do not merely relabel a BT.601 or full-range source as limited-range BT.709 to obtain acceleration; convert the actual image data when changing its matrix or range.

## NTSC-J Color Mode

`enable_ntscj_gamut_mode = true` is compatible with hardware decoding and does **not** disable it. Hardware decoding first produces an 8-bit RGB texture using limited-range BT.709 conversion. FFNx then applies its movie gamma and gamut processing, including the NTSC-J inverse conversions and final post-processing used to keep movies consistent with game graphics drawn over them.

Expect the following differences and limitations:

- NTSC-J mode honors supported movie primaries and transfer metadata; with the flag disabled, FFNx treats movie primaries and gamma as sRGB. See [Color Modes](color_modes.md) for the supported values and defaults.
- Native conversion has already reduced the movie to 8-bit RGB before FFNx's color processing. Hardware and software output need not be pixel-identical, particularly in gradients and after color correction. Enabling NTSC-J mode does not restore precision lost in that conversion.
- The left-sited chroma approximation applies with NTSC-J mode both enabled and disabled; the flag does not correct it.
- Enabling NTSC-J mode does not broaden hardware eligibility. A 10-bit, BT.601-matrix, or otherwise ineligible movie remains software-decoded.

Preserve metadata appropriate to the source rather than changing it solely for acceleration. NTSC-J mode intentionally changes how untagged original-style content is displayed; this is color processing, not a hardware-decoder conflict.

## FF8 Movies Above 15 FPS

FF8 field movies can be encoded at 30 or 60 FPS, with a `.cam` file that has one camera entry per video frame. Field scripts keep their original 15 FPS timing, and the 3D models drawn on top of the movie move smoothly between those steps.

## Checking the Selected Path

Enable `trace_movies = true` and inspect the runtime log:

- `Using Hardware video decoding` identifies hardware decoding.
- `Using Software video decoding` identifies software decoding.

Movie decoding failures and audio underruns are logged separately. Disable verbose movie tracing after diagnosis to avoid unnecessary logging overhead.