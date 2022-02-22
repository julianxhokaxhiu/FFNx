/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2022 Julian Xhokaxhiu                                   //
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

#include "movies.h"

// 10 frames
#define VIDEO_BUFFER_SIZE 10

#define LAG (((now - start_time) - (timer_freq / movie_fps) * movie_frame_counter) / (timer_freq / 1000))

uint32_t yuv_fast_path = false;

uint32_t audio_must_be_converted = false;

AVFormatContext *format_ctx = 0;
AVCodecContext *codec_ctx = 0;
AVCodec *codec = 0;
AVCodecContext *acodec_ctx = 0;
AVCodec *acodec = 0;
AVFrame *movie_frame = 0;
struct SwsContext *sws_ctx = 0;
SwrContext* swr_ctx = NULL;

int videostream;
int audiostream;

uint32_t use_bgra_texture;

struct video_frame
{
	uint32_t bgra_texture = 0;
	uint32_t yuv_textures[3] = { 0 };
};

struct video_frame video_buffer[VIDEO_BUFFER_SIZE];
uint32_t vbuffer_read = 0;
uint32_t vbuffer_write = 0;

uint32_t movie_frame_counter = 0;
uint32_t movie_frames;
uint32_t movie_width, movie_height;
double movie_fps;
double movie_duration;

bool first_audio_packet;

time_t timer_freq;
time_t start_time;

void ffmpeg_movie_init()
{
	ffnx_info("FFMpeg movie player plugin loaded\n");

	uint32_t texture_units = newRenderer.getCaps()->limits.maxTextureSamplers;

	if(texture_units < 3) ffnx_info("No multitexturing, codecs with YUV output will be slow. (texture units: %i)\n", texture_units);
	else yuv_fast_path = true;

	QueryPerformanceFrequency((LARGE_INTEGER *)&timer_freq);
}

// clean up anything we have allocated
void ffmpeg_release_movie_objects()
{
	uint32_t i;

	if (movie_frame) av_frame_free(&movie_frame);
	if (codec_ctx) avcodec_close(codec_ctx);
	if (acodec_ctx) avcodec_close(acodec_ctx);
	if (format_ctx) avformat_close_input(&format_ctx);
	if (swr_ctx) {
		swr_close(swr_ctx);
		swr_free(&swr_ctx);
	}

	codec_ctx = 0;
	acodec_ctx = 0;
	format_ctx = 0;

	audio_must_be_converted = false;

	for(i = 0; i < VIDEO_BUFFER_SIZE; i++)
	{
		// Cleanup BGRA textures
		newRenderer.deleteTexture(video_buffer[i].bgra_texture);
		video_buffer[i].bgra_texture = 0;

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
uint32_t ffmpeg_prepare_movie(char *name, bool with_audio)
{
	uint32_t i;
	WAVEFORMATEX sound_format;
	DSBUFFERDESC1 sbdesc;
	uint32_t ret;

	if(ret = avformat_open_input(&format_ctx, name, NULL, NULL))
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

	videostream = -1;
	audiostream = -1;
	for(i = 0; i < format_ctx->nb_streams; i++)
	{
		if(format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO && videostream < 0) videostream = i;
		if(with_audio && format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && audiostream < 0) audiostream = i;
	}

	if(videostream == -1)
	{
		ffnx_error("prepare_movie: no video stream found\n");
		ffmpeg_release_movie_objects();
		goto exit;
	}

	if(with_audio && audiostream == -1 && trace_movies) ffnx_trace("prepare_movie: no audio stream found\n");

	codec_ctx = format_ctx->streams[videostream]->codec;

	codec = avcodec_find_decoder(codec_ctx->codec_id);
	if(!codec)
	{
		ffnx_error("prepare_movie: no video codec found\n");
		codec_ctx = 0;
		ffmpeg_release_movie_objects();
		goto exit;
	}

	if(avcodec_open2(codec_ctx, codec, NULL) < 0)
	{
		ffnx_error("prepare_movie: couldn't open video codec\n");
		ffmpeg_release_movie_objects();
		goto exit;
	}

	if(audiostream != -1)
	{
		acodec_ctx = format_ctx->streams[audiostream]->codec;
		acodec = avcodec_find_decoder(acodec_ctx->codec_id);
		if(!acodec)
		{
			ffnx_error("prepare_movie: no audio codec found\n");
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

	movie_width = codec_ctx->width;
	movie_height = codec_ctx->height;
	movie_fps = 1.0 / (av_q2d(codec_ctx->time_base) * codec_ctx->ticks_per_frame);
	movie_duration = (double)format_ctx->duration / (double)AV_TIME_BASE;
	movie_frames = (uint32_t)::round(movie_fps * movie_duration);

	if (trace_movies)
	{
		if (movie_fps < 100.0) ffnx_info("prepare_movie: %s; %s/%s %ix%i, %f FPS, duration: %f, frames: %i, color_range: %d\n", name, codec->name, acodec_ctx ? acodec->name : "null", movie_width, movie_height, movie_fps, movie_duration, movie_frames, codec_ctx->color_range);
		// bogus FPS value, assume the codec provides frame limiting
		else ffnx_info("prepare_movie: %s; %s/%s %ix%i, duration: %f, color_range: %d\n", name, codec->name, acodec_ctx ? acodec->name : "null", movie_width, movie_height, movie_duration, codec_ctx->color_range);
	}

	if(movie_width > max_texture_size || movie_height > max_texture_size)
	{
		ffnx_error("prepare_movie: movie dimensions exceed max texture size, skipping\n");
		ffmpeg_release_movie_objects();
		goto exit;
	}

	if(!movie_frame) movie_frame = av_frame_alloc();

	if(sws_ctx) sws_freeContext(sws_ctx);

	if(codec_ctx->pix_fmt == AV_PIX_FMT_YUV420P && yuv_fast_path) use_bgra_texture = false;
	else use_bgra_texture = true;

	vbuffer_read = 0;
	vbuffer_write = 0;

	if(codec_ctx->pix_fmt != AV_PIX_FMT_BGRA && (codec_ctx->pix_fmt != AV_PIX_FMT_YUV420P || !yuv_fast_path))
	{
		sws_ctx = sws_getContext(movie_width, movie_height, codec_ctx->pix_fmt, movie_width, movie_height, AV_PIX_FMT_BGRA, SWS_FAST_BILINEAR | SWS_ACCURATE_RND, NULL, NULL, NULL);
		if (trace_movies) ffnx_info("prepare_movie: slow output format from video codec %s; %i\n", codec->name, codec_ctx->pix_fmt);
	}
	else sws_ctx = 0;

	if(audiostream != -1)
	{
		if (acodec_ctx->sample_fmt != AV_SAMPLE_FMT_FLT) {
			audio_must_be_converted = true;
			if (trace_movies)
			{
				ffnx_trace("prepare_movie: Audio must be converted: IN acodec_ctx->sample_fmt: %s\n", av_get_sample_fmt_name(acodec_ctx->sample_fmt));
				ffnx_trace("prepare_movie: Audio must be converted: IN acodec_ctx->sample_rate: %d\n", acodec_ctx->sample_rate);
				ffnx_trace("prepare_movie: Audio must be converted: IN acodec_ctx->channel_layout: %u\n", acodec_ctx->channel_layout);
				ffnx_trace("prepare_movie: Audio must be converted: IN acodec_ctx->channels: %u\n", acodec_ctx->channels);
			}

			// Prepare software conversion context
			swr_ctx = swr_alloc_set_opts(
				// Create a new context
				NULL,
				// OUT
				acodec_ctx->channel_layout == 0 ? AV_CH_LAYOUT_STEREO : acodec_ctx->channel_layout,
				AV_SAMPLE_FMT_FLT,
				acodec_ctx->sample_rate,
				// IN
				acodec_ctx->channel_layout == 0 ? AV_CH_LAYOUT_STEREO : acodec_ctx->channel_layout,
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
			acodec_ctx->channels
		);

		first_audio_packet = true;
	}

exit:
	movie_frame_counter = 0;

	return movie_frames;
}

// stop movie playback, no video updates will be requested after this so all we have to do is stop the audio
void ffmpeg_stop_movie()
{
	nxAudioEngine.stopStream();
}

void buffer_bgra_frame(uint8_t *data, int upload_stride)
{
	uint32_t upload_width = upload_stride;
	uint32_t tex_width = movie_width;
	uint32_t tex_height = movie_height;

	if(upload_stride < 0) return;

	if (video_buffer[vbuffer_write].bgra_texture)
		newRenderer.deleteTexture(video_buffer[vbuffer_write].bgra_texture);

	video_buffer[vbuffer_write].bgra_texture = newRenderer.createTexture(
		data,
		tex_width,
		tex_height,
		upload_width,
		RendererTextureType::BGRA
	);

	vbuffer_write = (vbuffer_write + 1) % VIDEO_BUFFER_SIZE;
}

void draw_bgra_frame(uint32_t buffer_index)
{
	newRenderer.isMovie(true);
	newRenderer.useTexture(video_buffer[buffer_index].bgra_texture);
	gl_draw_movie_quad(movie_width, movie_height);
	newRenderer.isMovie(false);
}

void upload_yuv_texture(uint8_t **planes, int *strides, uint32_t num, uint32_t buffer_index)
{
	uint32_t upload_width = strides[num];
	uint32_t tex_width = num == 0 ? movie_width : movie_width / 2;
	uint32_t tex_height = num == 0 ? movie_height : movie_height / 2;

	if (upload_width > tex_width) tex_width = upload_width;

	if (video_buffer[buffer_index].yuv_textures[num])
		newRenderer.deleteTexture(video_buffer[buffer_index].yuv_textures[num]);

	video_buffer[buffer_index].yuv_textures[num] = newRenderer.createTexture(
		planes[num],
		tex_width,
		tex_height,
		upload_width,
		RendererTextureType::YUV,
		false
	);
}

void buffer_yuv_frame(uint8_t **planes, int *strides)
{
	upload_yuv_texture(planes, strides, 0, vbuffer_write); // Y
	upload_yuv_texture(planes, strides, 1, vbuffer_write); // U
	upload_yuv_texture(planes, strides, 2, vbuffer_write); // V

	vbuffer_write = (vbuffer_write + 1) % VIDEO_BUFFER_SIZE;
}

void draw_yuv_frame(uint32_t buffer_index, bool full_range)
{
	for (uint32_t idx = 0; idx < 3; idx++)
		newRenderer.useTexture(video_buffer[buffer_index].yuv_textures[idx], idx);

	newRenderer.isMovie(true);
	newRenderer.isYUV(true);
	newRenderer.isFullRange(full_range);
	gl_draw_movie_quad(movie_width, movie_height);
	newRenderer.isFullRange(false);
	newRenderer.isYUV(false);
	newRenderer.isMovie(false);
}

// display the next frame
uint32_t ffmpeg_update_movie_sample(bool use_movie_fps)
{
	AVPacket packet;
	int ret;
	time_t now;
	DWORD DSStatus;

	// no playable movie loaded, skip it
	if(!format_ctx) return false;

	// keep track of when we started playing this movie
	if(movie_frame_counter == 0) QueryPerformanceCounter((LARGE_INTEGER *)&start_time);

	while((ret = av_read_frame(format_ctx, &packet)) >= 0)
	{
		if(packet.stream_index == videostream)
		{
			ret = avcodec_send_packet(codec_ctx, &packet);

			if (ret < 0)
			{
				ffnx_trace("%s: avcodec_send_packet -> %d\n", __func__, ret);
				av_packet_unref(&packet);
				break;
			}

			ret = avcodec_receive_frame(codec_ctx, movie_frame);

			if (ret == AVERROR_EOF)
			{
				ffnx_trace("%s: avcodec_receive_frame -> %d\n", __func__, ret);
				av_packet_unref(&packet);
				break;
			}

			if (ret >= 0)
			{
				QueryPerformanceCounter((LARGE_INTEGER *)&now);

				if(movie_sync_debug)
					ffnx_info("update_movie_sample(video): DTS %f PTS %f (timebase %f) placed in video buffer at real time %f (play %f)\n", (double)packet.dts, (double)packet.pts, av_q2d(codec_ctx->time_base), (double)(now - start_time) / (double)timer_freq, (double)movie_frame_counter / (double)movie_fps);

				if(sws_ctx)
				{
					uint8_t *planes[4] = { 0 };
					int strides[4] = { 0 };
					uint8_t *data = (uint8_t*)driver_calloc(movie_width * movie_height, 4);

					planes[0] = data;
					strides[0] = movie_width * 4;

					sws_scale(sws_ctx, movie_frame->extended_data, movie_frame->linesize, 0, movie_height, planes, strides);

					buffer_bgra_frame(data, movie_width * 4);

					driver_free(data);
				}
				else if(use_bgra_texture) buffer_bgra_frame(movie_frame->extended_data[0], movie_frame->linesize[0]);
				else buffer_yuv_frame(movie_frame->extended_data, movie_frame->linesize);

				if(vbuffer_write == vbuffer_read)
				{
					if(use_bgra_texture) draw_bgra_frame(vbuffer_read);
					else draw_yuv_frame(vbuffer_read, codec_ctx->color_range == AVCOL_RANGE_JPEG);

					vbuffer_read = (vbuffer_read + 1) % VIDEO_BUFFER_SIZE;

					av_packet_unref(&packet);

					break;
				}
			}
		}

		if(packet.stream_index == audiostream)
		{
			QueryPerformanceCounter((LARGE_INTEGER *)&now);

			ret = avcodec_send_packet(acodec_ctx, &packet);

			if (ret < 0)
			{
				ffnx_trace("%s: avcodec_send_packet -> %d\n", __func__, ret);
				av_packet_unref(&packet);
				break;
			}

			ret = avcodec_receive_frame(acodec_ctx, movie_frame);

			if (ret == AVERROR_EOF)
			{
				ffnx_trace("%s: avcodec_receive_frame -> %d\n", __func__, ret);
				av_packet_unref(&packet);
				break;
			}

			if (ret >= 0)
			{
				uint32_t bytesperpacket = audio_must_be_converted ? av_get_bytes_per_sample(AV_SAMPLE_FMT_FLT) : av_get_bytes_per_sample(acodec_ctx->sample_fmt);
				uint32_t _size = bytesperpacket * movie_frame->nb_samples * acodec_ctx->channels;

				// Sometimes the captured frame may have no sound samples. Just skip and move forward
				if (_size)
				{
					uint8_t *buffer;

					av_samples_alloc(&buffer, movie_frame->linesize, acodec_ctx->channels, movie_frame->nb_samples, (audio_must_be_converted ? AV_SAMPLE_FMT_FLT : acodec_ctx->sample_fmt), 0);
					if (audio_must_be_converted) swr_convert(swr_ctx, &buffer, movie_frame->nb_samples, (const uint8_t**)movie_frame->extended_data, movie_frame->nb_samples);
					else av_samples_copy(&buffer, movie_frame->extended_data, 0, 0, movie_frame->nb_samples, acodec_ctx->channels, acodec_ctx->sample_fmt);

					nxAudioEngine.pushStreamData(buffer, _size);
					nxAudioEngine.resumeStream();

					av_freep(&buffer);
				}
			}
		}

		av_packet_unref(&packet);
	}

	if (first_audio_packet)
	{
		first_audio_packet = false;

		// reset start time so video syncs up properly
		QueryPerformanceCounter((LARGE_INTEGER *)&start_time);

		nxAudioEngine.playStream(ff7_music_volume / 100.0f);
	}

	movie_frame_counter++;

	// could not read any more frames, exhaust video buffer then end movie
	if(ret < 0)
	{
		if(vbuffer_write != vbuffer_read)
		{
			if(use_bgra_texture) draw_bgra_frame(vbuffer_read);
			else draw_yuv_frame(vbuffer_read, codec_ctx->color_range == AVCOL_RANGE_JPEG);

			vbuffer_read = (vbuffer_read + 1) % VIDEO_BUFFER_SIZE;
		}

		if(vbuffer_write == vbuffer_read) return false;
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
	if(use_bgra_texture) draw_bgra_frame((vbuffer_read - 1) % VIDEO_BUFFER_SIZE);
	else draw_yuv_frame((vbuffer_read - 1) % VIDEO_BUFFER_SIZE, codec_ctx->color_range == AVCOL_RANGE_JPEG);
}

// loop back to the beginning of the movie
void ffmpeg_loop()
{
	if(format_ctx) avformat_seek_file(format_ctx, -1, 0, 0, 0, 0);
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
