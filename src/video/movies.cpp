/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2026 Julian Xhokaxhiu                                   //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#include "../audio.h"
#include "../renderer.h"
#include "../gl.h"

#include "movies.h"
#include "ntscj.h"

#include <deque>
#include <memory>

extern "C" {
#include <libavcodec/bsf.h>
}

// 10 frames
#define VIDEO_BUFFER_SIZE 10

#define LAG (((now - start_time) - (timer_freq / movie_fps) * movie_frame_counter) / (timer_freq / 1000))

uint32_t audio_must_be_converted = false;

AVFormatContext* format_ctx = nullptr;
AVCodecContext* codec_ctx = nullptr;
const AVCodec* codec = nullptr;
AVCodecContext* acodec_ctx = nullptr;
const AVCodec* acodec = nullptr;
AVFrame* movie_frame = nullptr;
AVFrame* sws_frame = nullptr;
AVFrame* usethis_frame = nullptr;
SwsContext* sws_ctx = nullptr;
SwsContext* usethis_sws = nullptr;
SwrContext* swr_ctx = nullptr;
void(*io_close)(void *opaque) = nullptr;
AVBSFContext* video_bsf = nullptr;
bgfx::TextureHandle decoded_video_texture = BGFX_INVALID_HANDLE;
bool video_decode_set = true;
bool video_decode_eof = false;
int64_t video_decode_origin = AV_NOPTS_VALUE;
int64_t video_decode_time_us = INT64_MIN;
uint64_t video_presentation_frame = 0;
int64_t video_presentation_time_us = 0;
float video_rgb_scale_x = 1.0f;
float video_rgb_scale_y = 1.0f;
std::deque<AVPacket> software_video_packets;
std::vector<uint8_t> video_bitstream;
std::vector<bgfx::VideoDecoderAu> video_access_units;
uint8_t* audio_conversion_buffer = nullptr;
unsigned int audio_conversion_buffer_size = 0;

int videostream;
int audiostream;

struct video_frame
{
	uint32_t yuv_textures[3] = { 0 };
	uint32_t texture_widths[3] = { 0 };
	uint32_t texture_heights[3] = { 0 };
	RendererTextureType texture_types[3] = {};
	bool eight_bit = false;
	bool interleaved = false;
	bool p010 = false;
	int64_t pts_us = AV_NOPTS_VALUE;
};

struct video_frame video_buffer[VIDEO_BUFFER_SIZE];
uint32_t vbuffer_read = 0;
uint32_t vbuffer_write = 0;

uint32_t movie_frame_counter = 0;
uint32_t movie_frames = 0;
uint32_t movie_width, movie_height;
double movie_fps;
double movie_duration;
const AVPixelFormat targetpixelformat = AV_PIX_FMT_YUV420P10;
bool dofirstframestuff = false;
bool needsws = false;
AVPixelFormat upload_pixel_format = targetpixelformat;

bool first_audio_packet;

time_t timer_freq;
time_t start_time;

static bool shader_supports_pixel_format(AVPixelFormat format)
{
	switch (format) {
		case AV_PIX_FMT_NV12:
		case AV_PIX_FMT_P010LE:
		case AV_PIX_FMT_YUV420P:
		case AV_PIX_FMT_YUV422P:
		case AV_PIX_FMT_YUV444P:
		case AV_PIX_FMT_YUVJ420P:
		case AV_PIX_FMT_YUVJ422P:
		case AV_PIX_FMT_YUVJ444P:
		case AV_PIX_FMT_YUV420P10LE:
		case AV_PIX_FMT_YUV422P10LE:
		case AV_PIX_FMT_YUV444P10LE:
			return true;
		default:
			return false;
	}
}

static bool bgfx_movie_format_is_eligible(const AVCodecParameters& parameters)
{
	return parameters.codec_id == AV_CODEC_ID_H264 &&
		parameters.format == AV_PIX_FMT_YUV420P &&
		parameters.color_space == AVCOL_SPC_BT709 &&
		parameters.color_range == AVCOL_RANGE_MPEG &&
		(parameters.chroma_location == AVCHROMA_LOC_CENTER || parameters.chroma_location == AVCHROMA_LOC_LEFT) &&
		(parameters.field_order == AV_FIELD_UNKNOWN || parameters.field_order == AV_FIELD_PROGRESSIVE);
}

static bool prepare_bgfx_decoder()
{
	const bgfx::Caps& caps = *newRenderer.getCaps();
	if (!(caps.supported & BGFX_CAPS_VIDEO_DECODE) ||
		!(caps.formats[bgfx::TextureFormat::RGBA8] & BGFX_CAPS_FORMAT_TEXTURE_VIDEO_DECODE_DST))
		return false;

	const AVStream& stream = *format_ctx->streams[videostream];
	const AVCodecParameters& parameters = *stream.codecpar;
	if (!bgfx_movie_format_is_eligible(parameters))
		return false;

	if (parameters.width <= 0 || parameters.height <= 0 ||
		parameters.width > max_texture_size || parameters.height > max_texture_size)
		return false;

	const uint32_t coded_width = (uint32_t(parameters.width) + 15) & ~15u;
	const uint32_t coded_height = (uint32_t(parameters.height) + 15) & ~15u;
	if (coded_width > max_texture_size || coded_height > max_texture_size ||
		coded_width > UINT16_MAX || coded_height > UINT16_MAX)
		return false;

	if (!bgfx::isVideoCodecValid(bgfx::VideoCodec::H264, 0, 8, coded_width, coded_height, 16, 16))
		return false;

	const AVBitStreamFilter* filter = av_bsf_get_by_name("h264_mp4toannexb");
	if (!filter)
		return false;

	AVBSFContext* filter_context = nullptr;
	if (av_bsf_alloc(filter, &filter_context) < 0)
		return false;

	const auto free_filter = [](AVBSFContext* context) { av_bsf_free(&context); };
	std::unique_ptr<AVBSFContext, decltype(free_filter)> bitstream_filter(filter_context, free_filter);
	if (avcodec_parameters_copy(bitstream_filter->par_in, &parameters) < 0)
		return false;

	bitstream_filter->time_base_in = stream.time_base;
	if (av_bsf_init(bitstream_filter.get()) < 0)
		return false;

	const AVCodecParameters& filtered_parameters = *bitstream_filter->par_out;
	if (!filtered_parameters.extradata || filtered_parameters.extradata_size <= 0)
		return false;

	const bgfx::Memory* memory = bgfx::alloc(sizeof(bgfx::VideoDecoderInit) + filtered_parameters.extradata_size);
	auto* init = reinterpret_cast<bgfx::VideoDecoderInit*>(memory->data);
	*init = {};
	init->magic = BX_MAKEFOURCC('V', 'D', 'I', 0);
	init->codec = bgfx::VideoCodec::H264;
	init->parameterSets = memory->data + sizeof(*init);
	init->parameterSetsSize = filtered_parameters.extradata_size;
	memcpy(memory->data + sizeof(*init), filtered_parameters.extradata, init->parameterSetsSize);

	const bgfx::TextureHandle texture = bgfx::createTexture2D(coded_width, coded_height, false, 1,
		bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_NONE, memory);
	if (!bgfx::isValid(texture))
		return false;

	video_bsf = bitstream_filter.release();
	decoded_video_texture = texture;
	video_rgb_scale_x = float(parameters.width) / coded_width;
	video_rgb_scale_y = float(parameters.height) / coded_height;
	video_decode_set = true;
	video_decode_eof = false;
	video_decode_origin = stream.start_time;
	video_decode_time_us = INT64_MIN;
	video_presentation_frame = 0;
	video_presentation_time_us = 0;
	bgfx::setName(decoded_video_texture, "Movie hardware decoder");
	return true;
}

static void submit_bgfx_video(const std::vector<uint8_t>& bitstream, const std::vector<bgfx::VideoDecoderAu>& access_units, int64_t presentation_time)
{
	const bgfx::Memory* memory = bgfx::alloc(sizeof(bgfx::VideoDecoderFrame) +
		access_units.size() * sizeof(bgfx::VideoDecoderAu) + bitstream.size());
	auto* frame = reinterpret_cast<bgfx::VideoDecoderFrame*>(memory->data);
	*frame = {};
	frame->magic = BX_MAKEFOURCC('V', 'D', 'F', 0);
	frame->presentationTimeUs = presentation_time;
	frame->flags = video_decode_eof ? BGFX_VIDEO_DECODE_FRAME_FINAL : BGFX_VIDEO_DECODE_FRAME_NONE;
	if (!access_units.empty()) {
		uint8_t* payload = memory->data + sizeof(*frame);
		frame->aus = reinterpret_cast<const bgfx::VideoDecoderAu*>(payload);
		frame->numAus = access_units.size();
		memcpy(payload, access_units.data(), access_units.size() * sizeof(bgfx::VideoDecoderAu));
		payload += access_units.size() * sizeof(bgfx::VideoDecoderAu);
		frame->bitstream = payload;
		memcpy(payload, bitstream.data(), bitstream.size());
		if (video_decode_set) {
			frame->flags |= BGFX_VIDEO_DECODE_FRAME_SET;
			video_decode_set = false;
		}
	}
	bgfx::updateTexture2D(decoded_video_texture, 0, 0, 0, 0, 1, 1, memory);
}

void prepare_sws_context(AVPixelFormat pixfmtin, AVColorSpace colorspacein){
	if (trace_movies || trace_all){
		if (pixfmtin != targetpixelformat){
			ffnx_trace("prepare_movie: Using swscale to convert pixel format from %s to %s.\n", av_get_pix_fmt_name(pixfmtin), av_get_pix_fmt_name(targetpixelformat));
		}
		if (!okcolorspace){
			ffnx_trace("prepare_movie: Using swscale to convert YUV colorspace from %s to rec601.\n", av_color_space_name(colorspacein));
		}
		if (yuvjfixneeded){
			ffnx_trace("prepare_movie: Using swscale to workaround ffmpeg yuvj color range bug.\n");
		}
	}
	// set up pixel format conversion
	// (scaling also crops green crap from padded bitstreams)
	SwsContext* this_sws_ctx = sws_getContext(
		movie_width,
		movie_height,
		pixfmtin,
		movie_width,
		movie_height,
		targetpixelformat,
		SWS_LANCZOS | SWS_ACCURATE_RND | SWS_FULL_CHR_H_INT,
		NULL,
		NULL,
		NULL
	);
	// if we need a colorspace conversion, set it up here
	// this would also be the place to set up color range conversion, if it worked -- which it doens't
	// so just suppress the buggy color range conversion if we can
	if (!okcolorspace || yuvjfixneeded){
		int *coefs_in;
		int *coefs_out;
		int srcRange, dstRange;
		int brightness, contrast, saturation;
		sws_getColorspaceDetails(this_sws_ctx, &coefs_in, &srcRange, &coefs_out, &dstRange, &brightness, &contrast, &saturation);

		coefs_in = const_cast<int*>(sws_getCoefficients(colorspacein)); // const sucks
		// use the same colorspace
		if (okcolorspace){
			coefs_out = coefs_in;
		}
		// convert
		else {
			coefs_out = const_cast<int*>(sws_getCoefficients(SWS_CS_ITU601)); // const sucks
		}

		// Surprisingly, these parameters don't appear to **do** anything in most cases.
		// It appears that whether swscale does a range conversion is controlled by pixformat and range metadata.
		// And it will do one regardless of whether you want it.
		// Except, when the input format is YUVJ, these parameters can be used to **prevent** an un-asked-for PC->TV conversion
		// (They are totally ignored with 10-bit input formats, however.)
		// Gawd... swscale is a buggy mess...
		if (yuvjfixneeded){
			srcRange = fullrange_input ? 1 : 0; // use the input color range
			dstRange = srcRange; // no conversion!
		}

		sws_setColorspaceDetails(this_sws_ctx, coefs_in, srcRange, coefs_out, dstRange, brightness, contrast, saturation);
	}
	sws_ctx = this_sws_ctx;
	return;
}

void ffmpeg_movie_init()
{
	ffnx_info("FFMpeg movie player plugin loaded\n");

	QueryPerformanceFrequency((LARGE_INTEGER *)&timer_freq);
}

// clean up anything we have allocated
void ffmpeg_release_movie_objects()
{
	uint32_t i;
	for (auto& queued_packet : software_video_packets) av_packet_unref(&queued_packet);
	software_video_packets.clear();
	std::vector<uint8_t>().swap(video_bitstream);
	std::vector<bgfx::VideoDecoderAu>().swap(video_access_units);
	av_freep(&audio_conversion_buffer);
	audio_conversion_buffer_size = 0;
	av_bsf_free(&video_bsf);
	if (bgfx::isValid(decoded_video_texture)) bgfx::destroy(decoded_video_texture);
	decoded_video_texture = BGFX_INVALID_HANDLE;
	video_decode_set = true;
	video_decode_eof = false;
	video_decode_origin = AV_NOPTS_VALUE;
	video_decode_time_us = INT64_MIN;
	video_presentation_frame = 0;
	video_presentation_time_us = 0;

	if (movie_frame) av_frame_free(&movie_frame);
	if (sws_frame) av_frame_free(&sws_frame);
	if (codec_ctx) avcodec_free_context(&codec_ctx);
	if (acodec_ctx) avcodec_free_context(&acodec_ctx);
	if (format_ctx) {
		if (format_ctx->flags & AVFMT_FLAG_CUSTOM_IO) {
			AVIOContext *pb = format_ctx->pb;
			if (pb) {
				if (io_close) {
					io_close(pb->opaque);
				}
				av_freep(&pb->buffer);
				av_free(pb);
			}
		}
		avformat_close_input(&format_ctx);
	}
	if (swr_ctx) {
		swr_close(swr_ctx);
		swr_free(&swr_ctx);
	}
	if(sws_ctx) sws_freeContext(sws_ctx);

	codec_ctx = nullptr;
	acodec_ctx = nullptr;
	format_ctx = nullptr;
	sws_ctx = nullptr;

	audio_must_be_converted = false;

	for(i = 0; i < VIDEO_BUFFER_SIZE; i++)
	{
		// Cleanup YUV textures
		for (uint32_t idx = 0; idx < 3; idx++)
		{
			newRenderer.deleteTexture(video_buffer[i].yuv_textures[idx]);
			video_buffer[i].yuv_textures[idx] = 0;
		}
	}

	// Unset slot U and V as they are used only for YUV textures
	newRenderer.useTexture(0, RendererTextureSlot::TEX_U);
	newRenderer.useTexture(0, RendererTextureSlot::TEX_V);
}

// prepare a movie for playback
uint32_t ffmpeg_prepare_movie(const char *name, bool with_audio)
{
	uint32_t i;
	WAVEFORMATEX sound_format;
	DSBUFFERDESC1 sbdesc;
	bool okpixelformat = false;
	bool islogomovie = false;
	AVPixelFormat nativepixformat = AV_PIX_FMT_NONE;

	movie_frames = 0;
	first_audio_packet = false;

	if(avformat_open_input(&format_ctx, name, NULL, NULL))
	{
		ffnx_error("prepare_movie: couldn't open movie file: %s\n", name);
		ffmpeg_release_movie_objects();
		goto exit;
	}

	if(avformat_find_stream_info(format_ctx, NULL) < 0)
	{
		ffnx_error("prepare_movie: couldn't find stream info\n");
		ffmpeg_release_movie_objects();
		goto exit;
	}

	videostream = av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
	if (videostream < 0) {
		ffnx_error("prepare_movie: no video stream found\n");
		ffmpeg_release_movie_objects();
		goto exit;
	}

	if (with_audio)
	{
		audiostream = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &acodec, 0);
		if(audiostream < 0 && (trace_movies || trace_all)) ffnx_trace("prepare_movie: no audio stream found\n");
	}
	else
		audiostream = -1;

	prepare_bgfx_decoder();
	if (trace_movies || trace_all) ffnx_trace("prepare_movie: Using %s video decoding\n", bgfx::isValid(decoded_video_texture) ? "Hardware" : "Software");

	codec_ctx = avcodec_alloc_context3(codec);
	if (!codec_ctx) {
		ffnx_error("prepare_movie: could not allocate video codec context\n");
		codec_ctx = 0;
		ffmpeg_release_movie_objects();
		goto exit;
	}
	avcodec_parameters_to_context(codec_ctx, format_ctx->streams[videostream]->codecpar);

	if(avcodec_open2(codec_ctx, codec, NULL) < 0)
	{
		ffnx_error("prepare_movie: couldn't open video codec\n");
		ffmpeg_release_movie_objects();
		goto exit;
	}

	if(audiostream >= 0)
	{
		acodec_ctx = avcodec_alloc_context3(acodec);
		if (!acodec_ctx) {
			ffnx_error("prepare_movie: could not allocate audio codec context\n");
			codec_ctx = 0;
			ffmpeg_release_movie_objects();
			goto exit;
		}
		if (avcodec_parameters_to_context(acodec_ctx, format_ctx->streams[audiostream]->codecpar) < 0) {
			ffnx_error("prepare_movie: could not fill the codec context from the codec parameters\n");
			ffmpeg_release_movie_objects();
			goto exit;
		}
		if(avcodec_open2(acodec_ctx, acodec, NULL) < 0)
		{
			ffnx_error("prepare_movie: couldn't open audio codec\n");
			ffmpeg_release_movie_objects();
			goto exit;
		}
	}

	islogomovie = ntscj_detect_logo_movie(name, codec_ctx);

	movie_width = codec_ctx->width;
	movie_height = codec_ctx->height;
	movie_fps = av_q2d(av_guess_frame_rate(format_ctx, format_ctx->streams[videostream], NULL));
	movie_duration = (double)format_ctx->duration / (double)AV_TIME_BASE;
	movie_frames = (uint32_t)::round(movie_fps * movie_duration);

	nativepixformat = codec_ctx->pix_fmt;
	ntscj_prepare_movie_colors(codec_ctx, islogomovie);

	// will we need to convert the pixel format?
	// We're going to target YUV420 on the assumption that most of our input will already be 420,
	// and resampling chroma in the shader will be faster than doing it in swscale.
	// (Also, we generally shouldn't target a YUVJ format because that triggers a bunch of automatic, sometimes wrong, color range conversions.)
	// We'll target 10-bit so we don't lose data when input is 10-bit,
	// to speed up our most common and slowest input case by not needing swscale,
	// and opening the door to HDR video someday.
	okpixelformat = bgfx::isValid(decoded_video_texture) || shader_supports_pixel_format(nativepixformat);

	if (trace_movies || trace_all)
	{
		if (movie_fps < 100.0) ffnx_info("prepare_movie: %s; %s/%s %ix%i (coded %ix%i), %f FPS, duration: %f, frames: %i, pixel format: %s (needs conversion: %i), chroma location: %i, color matrix: %s (shader matrix type %i)(needs conversion: %i), color_range: %d (needs yuvj fix: %i), transfer function %s (shader gamma type: %i), color primaries: %s (shader color gamut: %i), islogomovie: %i\n", name, codec->name, acodec_ctx ? acodec->name : "null", movie_width, movie_height, codec_ctx->coded_width, codec_ctx->coded_height, movie_fps, movie_duration, movie_frames, av_get_pix_fmt_name(nativepixformat), !okpixelformat, chromaloc, av_color_space_name(codec_ctx->colorspace), colormatrix, !okcolorspace, codec_ctx->color_range, yuvjfixneeded, av_color_transfer_name(codec_ctx->color_trc), gammatype, av_color_primaries_name(codec_ctx->color_primaries), colorgamut, islogomovie);
		else ffnx_info("prepare_movie: %s; %s/%s %ix%i (coded %ix%i), duration: %f, frames: %i, pixel format: %s (needs conversion: %i), chroma location: %i, color matrix: %s (shader matrix type %i)(needs conversion: %i), color_range: %d (needs yuvj fix: %i), transfer function %s (shader gamma type: %i), color primaries: %s (shader color gamut: %i), islogomovie: %i\n", name, codec->name, acodec_ctx ? acodec->name : "null", movie_width, movie_height, codec_ctx->coded_width, codec_ctx->coded_height, movie_duration, movie_frames, av_get_pix_fmt_name(nativepixformat), !okpixelformat, chromaloc, av_color_space_name(codec_ctx->colorspace), colormatrix, !okcolorspace, codec_ctx->color_range, yuvjfixneeded, av_color_transfer_name(codec_ctx->color_trc), gammatype, av_color_primaries_name(codec_ctx->color_primaries), colorgamut, islogomovie);
	}

	if(movie_width > max_texture_size || movie_height > max_texture_size)
	{
		ffnx_error("prepare_movie: movie dimensions exceed max texture size, skipping\n");
		ffmpeg_release_movie_objects();
		goto exit;
	}

	if(!movie_frame) movie_frame = av_frame_alloc();
	if(!sws_frame) sws_frame = av_frame_alloc();

	vbuffer_read = 0;
	vbuffer_write = 0;

	dofirstframestuff = true;

	// Make sure the swscale context is cleared out first
	if(sws_ctx) sws_freeContext(sws_ctx);
	sws_ctx = nullptr;
	needsws = !okpixelformat || !okcolorspace;

	// if we're using swscale to fix the color matrix, tell the shader it's 601
	if (!okcolorspace){
		colormatrix = COLORMATRIX_BT601;
	}

	if(audiostream >= 0)
	{
		if (acodec_ctx->sample_fmt != AV_SAMPLE_FMT_FLT) {
			audio_must_be_converted = true;
			if (trace_movies || trace_all)
			{
				ffnx_trace("prepare_movie: Audio must be converted: IN acodec_ctx->sample_fmt: %s\n", av_get_sample_fmt_name(acodec_ctx->sample_fmt));
				ffnx_trace("prepare_movie: Audio must be converted: IN acodec_ctx->sample_rate: %d\n", acodec_ctx->sample_rate);
				ffnx_trace("prepare_movie: Audio must be converted: IN acodec_ctx->ch_layout.u.mask: %u\n", acodec_ctx->ch_layout.u.mask);
				ffnx_trace("prepare_movie: Audio must be converted: IN acodec_ctx->ch_layout.nb_channels: %u\n", acodec_ctx->ch_layout.nb_channels);
			}

			// Prepare software conversion context
			swr_alloc_set_opts2(
				// Create a new context
				&swr_ctx,
				// OUT
				&acodec_ctx->ch_layout,
				AV_SAMPLE_FMT_FLT,
				acodec_ctx->sample_rate,
				// IN
				&acodec_ctx->ch_layout,
				acodec_ctx->sample_fmt,
				acodec_ctx->sample_rate,
				// LOG
				0,
				NULL
			);

			swr_init(swr_ctx);
		}

		nxAudioEngine.initStream(
			movie_duration,
			acodec_ctx->sample_rate,
			acodec_ctx->ch_layout.nb_channels
		);

		first_audio_packet = true;
	}

	exit:
	movie_frame_counter = 0;

	return movie_frames;
}

uint32_t ffmpeg_prepare_movie_from_io(
	const char* name, void *opaque,
	int(*read_packet)(void *, uint8_t *, int),
	int64_t(*seek)(void *, int64_t, int),
	void(*close)(void *opaque), bool with_audio)
{
	format_ctx = avformat_alloc_context();
	if (format_ctx == nullptr)
	{
		return 0;
	}

	const int buffer_size = 32768;
	uint8_t* buffer = (uint8_t*)av_malloc(buffer_size);

	if (buffer == nullptr)
	{
		ffmpeg_release_movie_objects();

		return 0;
	}

	AVIOContext* io_ctx = avio_alloc_context(buffer, buffer_size, 0, opaque, read_packet, nullptr, seek);

	if (io_ctx == nullptr)
	{
		ffmpeg_release_movie_objects();

		delete[] buffer;

		return 0;
	}

	format_ctx->pb = io_ctx;
	io_close = close;

	return ffmpeg_prepare_movie(name, with_audio);
}

// stop movie playback, no video updates will be requested after this so all we have to do is stop the audio
void ffmpeg_stop_movie()
{
	nxAudioEngine.stopStream();
}

void upload_yuv_texture(uint8_t **planes, int *strides, uint32_t num, uint32_t buffer_index)
{
	uint32_t upload_width = strides[num];
	uint32_t tex_width = movie_width;
	uint32_t tex_height = movie_height;
	// Match chroma plane dimensions to the decoded format.
	if (num > 0){
		const AVPixFmtDescriptor* descriptor = av_pix_fmt_desc_get(upload_pixel_format);
		tex_width = AV_CEIL_RSHIFT(tex_width, descriptor->log2_chroma_w);
		tex_height = AV_CEIL_RSHIFT(tex_height, descriptor->log2_chroma_h);
	}

	video_frame& frame = video_buffer[buffer_index];
	const RendererTextureType texture_type = (num > 0 && frame.interleaved) ?
		(frame.eight_bit ? RendererTextureType::UV8 : RendererTextureType::UV16) :
		(frame.eight_bit ? RendererTextureType::YUV8 : RendererTextureType::YUV);
	const bgfx::TextureHandle texture = { uint16_t(frame.yuv_textures[num]) };
	if (frame.yuv_textures[num] && bgfx::isValid(texture) &&
		frame.texture_widths[num] == tex_width && frame.texture_heights[num] == tex_height &&
		frame.texture_types[num] == texture_type) {
		const uint32_t upload_size = upload_width * tex_height;
		if (planes[num] && Renderer::doesItFitInMemory(upload_size)) {
			const bgfx::Memory* memory = bgfx::copy(planes[num], upload_size);
			if (memory)
				bgfx::updateTexture2D(texture, 0, 0, 0, 0, tex_width, tex_height, memory, upload_width);
		}
		return;
	}

	if (frame.yuv_textures[num])
		newRenderer.deleteTexture(frame.yuv_textures[num]);

	frame.yuv_textures[num] = newRenderer.createTexture(
		planes[num],
		tex_width,
		tex_height,
		upload_width,
		texture_type,
		false
	);
	frame.texture_widths[num] = tex_width;
	frame.texture_heights[num] = tex_height;
	frame.texture_types[num] = texture_type;
}

void buffer_yuv_frame(uint8_t **planes, int *strides)
{
	const AVPixFmtDescriptor* descriptor = av_pix_fmt_desc_get(upload_pixel_format);
	const AVStream* stream = format_ctx->streams[videostream];
	const int64_t origin = stream->start_time == AV_NOPTS_VALUE ? 0 : stream->start_time;
	video_buffer[vbuffer_write].pts_us = movie_frame->best_effort_timestamp == AV_NOPTS_VALUE ? AV_NOPTS_VALUE :
		av_rescale_q(movie_frame->best_effort_timestamp - origin, stream->time_base, AV_TIME_BASE_Q);
	video_buffer[vbuffer_write].eight_bit = descriptor->comp[0].depth == 8;
	video_buffer[vbuffer_write].interleaved = upload_pixel_format == AV_PIX_FMT_NV12 || upload_pixel_format == AV_PIX_FMT_P010LE;
	video_buffer[vbuffer_write].p010 = upload_pixel_format == AV_PIX_FMT_P010LE;
	upload_yuv_texture(planes, strides, 0, vbuffer_write); // Y
	upload_yuv_texture(planes, strides, 1, vbuffer_write); // U
	if (!video_buffer[vbuffer_write].interleaved) upload_yuv_texture(planes, strides, 2, vbuffer_write); // V
	else if (video_buffer[vbuffer_write].yuv_textures[2]) {
		newRenderer.deleteTexture(video_buffer[vbuffer_write].yuv_textures[2]);
		video_buffer[vbuffer_write].yuv_textures[2] = 0;
	}
	vbuffer_write = (vbuffer_write + 1) % VIDEO_BUFFER_SIZE;
}

void draw_yuv_frame(uint32_t buffer_index)
{
	if(gl_defer_yuv_frame(buffer_index)) return;

	bool rgb_movie = bgfx::isValid(decoded_video_texture);
	if (rgb_movie) {
		newRenderer.useTexture(decoded_video_texture.idx);
		newRenderer.useTexture(0, RendererTextureSlot::TEX_U);
		newRenderer.useTexture(0, RendererTextureSlot::TEX_V);
	}
	else {
		for (uint32_t idx = 0; idx < 3; idx++)
			newRenderer.useTexture(video_buffer[buffer_index].yuv_textures[idx], idx);
	}

	newRenderer.isMovie(true);
	newRenderer.setMovieRGB(rgb_movie, video_rgb_scale_x, video_rgb_scale_y);
	newRenderer.setMovieYUVFormat(video_buffer[buffer_index].eight_bit, video_buffer[buffer_index].interleaved, video_buffer[buffer_index].p010);
	newRenderer.isFullRange(fullrange_input);
	newRenderer.setColorMatrix(colormatrix);
	newRenderer.setColorGamut(colorgamut);
	newRenderer.setGammaType(gammatype);
	newRenderer.setChromaLocationType(chromaloc);
	gl_draw_movie_quad(movie_width, movie_height);
	newRenderer.isMovie(false);
	newRenderer.setMovieRGB();
	newRenderer.setMovieYUVFormat();
	// set these back to default
	newRenderer.isFullRange();
	newRenderer.setColorMatrix();
	newRenderer.setColorGamut();
	newRenderer.setGammaType();
	newRenderer.setChromaLocationType();
}

// display the next frame
uint32_t ffmpeg_update_movie_sample(bool use_movie_fps)
{
	AVPacket packet{};
	int ret = 0;
	time_t now;
	video_bitstream.clear();
	video_access_units.clear();
	int64_t presentation_time = movie_fps > 0.0 ? int64_t(movie_frame_counter * 1000000.0 / movie_fps) : 0;
	const bool native_video = bgfx::isValid(decoded_video_texture);
	const double audio_buffer_target = 0.5;
	bool software_frame_drawn = false;

	// no playable movie loaded, skip it
	if(!format_ctx) return false;

	// keep track of when we started playing this movie
	if(movie_frame_counter == 0) QueryPerformanceCounter((LARGE_INTEGER *)&start_time);
	if (!first_audio_packet) {
		QueryPerformanceCounter((LARGE_INTEGER *)&now);
		presentation_time = int64_t(double(now - start_time) * 1000000.0 / timer_freq);
		double audio_time = audiostream >= 0 ? nxAudioEngine.getStreamPlaybackTime() : -1.0;
		if (audio_time >= 0.0) {
			const int64_t audio_time_us = int64_t(audio_time * 1000000.0);
			if (native_video) presentation_time = audio_time_us;
			else {
				const int64_t buffer_duration_us = int64_t(nxAudioEngine.getStreamBufferDuration() * 1000000.0);
				presentation_time = std::max(audio_time_us - buffer_duration_us,
					std::min(audio_time_us + buffer_duration_us, presentation_time));
			}
		}
		if (native_video && presentation_time < video_presentation_time_us) presentation_time = video_presentation_time_us;
	}
	const int64_t video_read_target = presentation_time + (movie_fps > 0.0 ? int64_t(VIDEO_BUFFER_SIZE * 1000000.0 / movie_fps) : 0);

	if (!native_video && movie_frame_counter > 0 && vbuffer_read != vbuffer_write &&
		video_buffer[vbuffer_read].pts_us != AV_NOPTS_VALUE && video_buffer[vbuffer_read].pts_us > presentation_time) {
		draw_yuv_frame((vbuffer_read + VIDEO_BUFFER_SIZE - 1) % VIDEO_BUFFER_SIZE);
		software_frame_drawn = true;
	}

	while ((native_video ? video_decode_time_us < video_read_target : !software_frame_drawn) ||
		(audiostream >= 0 && nxAudioEngine.getStreamBufferedDuration() < audio_buffer_target))
	{
		if (!native_video && !software_frame_drawn && !software_video_packets.empty()) {
			av_packet_move_ref(&packet, &software_video_packets.front());
			software_video_packets.pop_front();
			ret = 0;
		}
		else ret = av_read_frame(format_ctx, &packet);
		if (ret < 0) break;

		if(packet.stream_index == videostream)
		{
			if (!native_video && software_frame_drawn) {
				software_video_packets.emplace_back();
				av_packet_move_ref(&software_video_packets.back(), &packet);
				continue;
			}
			if (bgfx::isValid(decoded_video_texture)) {
				if (video_decode_set && video_access_units.empty() && !(packet.flags & AV_PKT_FLAG_KEY)) {
					av_packet_unref(&packet);
					continue;
				}
				ret = av_bsf_send_packet(video_bsf, &packet);
				if (ret < 0) {
					ffnx_error("%s: video bitstream filter send failed: %d\n", __func__, ret);
					av_packet_unref(&packet);
					goto packet_loop_exit;
				}
				AVPacket filtered_packet{};
				while ((ret = av_bsf_receive_packet(video_bsf, &filtered_packet)) >= 0) {
					int64_t timestamp = filtered_packet.pts != AV_NOPTS_VALUE ? filtered_packet.pts : filtered_packet.dts;
					if (timestamp == AV_NOPTS_VALUE) {
						ffnx_error("%s: native video decoding requires packet timestamps\n", __func__);
						av_packet_unref(&filtered_packet);
						ret = AVERROR_INVALIDDATA;
						goto packet_loop_exit;
					}
					if (video_decode_origin == AV_NOPTS_VALUE) video_decode_origin = timestamp;
					int64_t pts_us = av_rescale_q(timestamp - video_decode_origin, video_bsf->time_base_out, AV_TIME_BASE_Q);
					video_decode_time_us = filtered_packet.dts == AV_NOPTS_VALUE ? pts_us :
						av_rescale_q(filtered_packet.dts - video_decode_origin, video_bsf->time_base_out, AV_TIME_BASE_Q);
					video_access_units.push_back({ uint32_t(filtered_packet.size), pts_us });
					video_bitstream.insert(video_bitstream.end(), filtered_packet.data, filtered_packet.data + filtered_packet.size);
					av_packet_unref(&filtered_packet);
				}
				if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
					ffnx_error("%s: video bitstream filter receive failed: %d\n", __func__, ret);
					goto packet_loop_exit;
				}
				ret = 0;
				continue;
			}
			ret = avcodec_send_packet(codec_ctx, &packet);

			if (ret < 0)
			{
				ffnx_trace("%s: avcodec_send_packet (video) -> %d\n", __func__, ret);
				av_packet_unref(&packet);
				break;
			}

			// Receive all frames produced by this packet
			while (true)
			{
				ret = avcodec_receive_frame(codec_ctx, movie_frame);

				if (ret == AVERROR(EAGAIN))
				{
					// Decoder needs more input, continue to next packet
					break;
				}

				if (ret < 0)
				{
					if (ret != AVERROR_EOF) ffnx_trace("%s: avcodec_receive_frame (video) -> %d\n", __func__, ret);
					av_packet_unref(&packet);
					goto packet_loop_exit;
				}

				// use a pointer to identify the AVFrame object we ultimately want to use
				// so we can call buffer_yuv_frame() in just one place using this pointer
				usethis_frame = movie_frame;
				// ditto for sws context
				usethis_sws = sws_ctx;

				needsws = !okcolorspace || !shader_supports_pixel_format(AVPixelFormat(usethis_frame->format)) ||
					usethis_frame->linesize[0] <= 0 || usethis_frame->linesize[1] <= 0 ||
					(usethis_frame->data[2] && usethis_frame->linesize[2] <= 0);
				if (needsws && !usethis_sws){
					prepare_sws_context(AVPixelFormat(usethis_frame->format), codec_ctx->colorspace);
					usethis_sws = sws_ctx;
					if (dofirstframestuff && (trace_movies || trace_all)){
						ffnx_trace("ffmpeg_update_movie_sample: Using swscale for unsupported pixel format, colorspace, or negative stride.\n");
					}
				}

				// if we need to use swscale, do so
				if(needsws)
				{
					if (!sws_frame->buf[0]) {
						sws_frame->width = movie_width;
						sws_frame->height = movie_height;
						sws_frame->format = targetpixelformat;
						ret = av_frame_get_buffer(sws_frame, 0);
						if (ret < 0) {
							ffnx_error("%s: could not allocate movie conversion frame: %d\n", __func__, ret);
							av_packet_unref(&packet);
							goto packet_loop_exit;
						}
					}
					ret = av_frame_make_writable(sws_frame);
					if (ret < 0) {
						ffnx_error("%s: could not make movie conversion frame writable: %d\n", __func__, ret);
						av_packet_unref(&packet);
						goto packet_loop_exit;
					}

					sws_scale(usethis_sws, usethis_frame->extended_data, usethis_frame->linesize, 0, sws_frame->height, sws_frame->data, sws_frame->linesize);

					// swap the pointer's target
					usethis_frame = sws_frame;
				}
				// send frame data to buffer
				upload_pixel_format = AVPixelFormat(usethis_frame->format);
				buffer_yuv_frame(usethis_frame->extended_data, usethis_frame->linesize);

				// clear out the AVFrame objects before reusing them
				av_frame_unref(movie_frame);

				dofirstframestuff = false;

				if(vbuffer_write == vbuffer_read)
				{
					const uint32_t next_frame = (vbuffer_read + 1) % VIDEO_BUFFER_SIZE;
					if (video_buffer[next_frame].pts_us != AV_NOPTS_VALUE && video_buffer[next_frame].pts_us <= presentation_time) {
						vbuffer_read = next_frame;
						continue;
					}
					draw_yuv_frame(vbuffer_read);

					vbuffer_read = (vbuffer_read + 1) % VIDEO_BUFFER_SIZE;

					software_frame_drawn = true;
					break;
				}
			}
		}

		if(packet.stream_index == audiostream)
		{
			ret = avcodec_send_packet(acodec_ctx, &packet);

			if (ret < 0)
			{
				ffnx_trace("%s: avcodec_send_packet (audio) -> %d\n", __func__, ret);
				av_packet_unref(&packet);
				break;
			}

			// Receive all frames produced by this packet
			while (true)
			{
				ret = avcodec_receive_frame(acodec_ctx, movie_frame);

				if (ret == AVERROR(EAGAIN))
				{
					// Decoder needs more input, continue to next packet
					break;
				}

				if (ret < 0)
				{
					if (ret != AVERROR_EOF) ffnx_trace("%s: avcodec_receive_frame (audio) -> %d\n", __func__, ret);
					av_packet_unref(&packet);
					goto packet_loop_exit;
				}

				// Successfully received a frame
				uint32_t bytesperpacket = audio_must_be_converted ? av_get_bytes_per_sample(AV_SAMPLE_FMT_FLT) : av_get_bytes_per_sample(acodec_ctx->sample_fmt);
				uint32_t _size = bytesperpacket * movie_frame->nb_samples * acodec_ctx->ch_layout.nb_channels;

				// Sometimes the captured frame may have no sound samples. Just skip and move forward
				if (_size)
				{
					uint8_t* buffer = movie_frame->extended_data[0];
					int converted_samples = movie_frame->nb_samples;
					if (audio_must_be_converted) {
						av_fast_malloc(&audio_conversion_buffer, &audio_conversion_buffer_size, _size);
						if (!audio_conversion_buffer) {
							ret = AVERROR(ENOMEM);
							ffnx_error("%s: could not allocate movie audio samples: %d\n", __func__, ret);
							av_packet_unref(&packet);
							goto packet_loop_exit;
						}
						buffer = audio_conversion_buffer;
						converted_samples = swr_convert(swr_ctx, &buffer, movie_frame->nb_samples, (const uint8_t**)movie_frame->extended_data, movie_frame->nb_samples);
						ret = converted_samples;
						if (ret < 0) {
							ffnx_error("%s: could not convert movie audio samples: %d\n", __func__, ret);
							av_packet_unref(&packet);
							goto packet_loop_exit;
						}
					}
					_size = bytesperpacket * converted_samples * acodec_ctx->ch_layout.nb_channels;
					if (_size) nxAudioEngine.pushStreamData(buffer, _size);
				}
			}
		}

		av_packet_unref(&packet);
		if (native_video) ret = 0;
	}

packet_loop_exit:

	if (bgfx::isValid(decoded_video_texture)) {
		if (ret == AVERROR_EOF) video_decode_eof = true;
		else if (ret < 0) return false;
		if (video_decode_eof && video_access_units.empty() && presentation_time >= int64_t(movie_duration * 1000000.0)) {
			movie_frame_counter = movie_frames;
			return false;
		}
		if (!video_access_units.empty())
			submit_bgfx_video(video_bitstream, video_access_units, video_presentation_time_us);
		if (movie_fps > 0.0) {
			int64_t tick_time = int64_t(ceil(video_presentation_frame * 1000000.0 / movie_fps));
			while (tick_time <= presentation_time) {
				submit_bgfx_video({}, {}, tick_time);
				video_presentation_time_us = tick_time;
				video_presentation_frame++;
				tick_time = int64_t(ceil(video_presentation_frame * 1000000.0 / movie_fps));
			}
		}
		else submit_bgfx_video({}, {}, presentation_time);
		draw_yuv_frame(0);
		ret = 0;
	}

	if (first_audio_packet && nxAudioEngine.getStreamBufferedDuration() > 0.0)
	{
		first_audio_packet = false;

		// reset start time so video syncs up properly
		QueryPerformanceCounter((LARGE_INTEGER *)&start_time);

		nxAudioEngine.playStream();
	}

	if (native_video) movie_frame_counter = uint32_t(video_presentation_frame);

	// could not read any more frames, exhaust video buffer then end movie
	if(ret < 0 && !software_frame_drawn)
	{
		if(vbuffer_write != vbuffer_read)
		{
			uint32_t next_frame = (vbuffer_read + 1) % VIDEO_BUFFER_SIZE;
			while (next_frame != vbuffer_write && video_buffer[next_frame].pts_us != AV_NOPTS_VALUE &&
				video_buffer[next_frame].pts_us <= presentation_time) {
				vbuffer_read = next_frame;
				next_frame = (vbuffer_read + 1) % VIDEO_BUFFER_SIZE;
			}
			draw_yuv_frame(vbuffer_read);

			vbuffer_read = (vbuffer_read + 1) % VIDEO_BUFFER_SIZE;
		}

		if(vbuffer_write == vbuffer_read) {
			movie_frame_counter = movie_frames;
			return false;
		}
	}

	if (!native_video) {
		const int64_t frame_time_us = video_buffer[(vbuffer_read + VIDEO_BUFFER_SIZE - 1) % VIDEO_BUFFER_SIZE].pts_us;
		if (frame_time_us != AV_NOPTS_VALUE && frame_time_us >= 0 && movie_fps > 0.0)
			movie_frame_counter = uint32_t(frame_time_us * movie_fps / 1000000.0 + 0.5) + 1;
		else movie_frame_counter++;
	}

	// Pure movie playback has no frame limiter, although it is not always required. Use it only when necessary
	if (use_movie_fps)
	{
		// wait for the next frame
		do
		{
			QueryPerformanceCounter((LARGE_INTEGER *)&now);
		} while(LAG < 0.0);
	}

	// keep going
	return true;
}

// draw the current frame, don't update anything
void ffmpeg_draw_current_frame()
{
	draw_yuv_frame((vbuffer_read - 1) % VIDEO_BUFFER_SIZE);
}

// loop back to the beginning of the movie
void ffmpeg_loop()
{
	for (auto& queued_packet : software_video_packets) av_packet_unref(&queued_packet);
	software_video_packets.clear();
	if(format_ctx) avformat_seek_file(format_ctx, -1, 0, 0, 0, 0);
	if (codec_ctx) avcodec_flush_buffers(codec_ctx);
	if (acodec_ctx) avcodec_flush_buffers(acodec_ctx);
	if (movie_frame) av_frame_unref(movie_frame);
	vbuffer_read = 0;
	vbuffer_write = 0;
	movie_frame_counter = 0;
	first_audio_packet = audiostream >= 0;
	if (first_audio_packet) {
		nxAudioEngine.stopStream();
		nxAudioEngine.initStream(movie_duration, acodec_ctx->sample_rate, acodec_ctx->ch_layout.nb_channels);
	}
	if (video_bsf) {
		av_bsf_flush(video_bsf);
		video_decode_set = true;
		video_decode_eof = false;
		video_decode_time_us = INT64_MIN;
		video_presentation_frame = 0;
		video_presentation_time_us = 0;
	}
}

// get the current frame number
uint32_t ffmpeg_get_movie_frame()
{
	return movie_frame_counter;
}

short ffmpeg_get_fps_ratio()
{
	return ceil(movie_fps / 15.0f);
}
