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

#include "vgmstream.h"
#include "../../utils.h"
#include "./zzzstreamfile.h"

namespace SoLoud
{
	VGMStreamInstance::VGMStreamInstance(VGMStream* aParent)
	{
		mParent = aParent;
		mStreamBuffer = new sample_t[SAMPLE_GRANULARITY * aParent->mChannels];

		rewind();
	}

	VGMStreamInstance::~VGMStreamInstance()
	{
		delete[] mStreamBuffer;
	}

	unsigned int VGMStreamInstance::getAudio(float* aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize)
	{
		memset(mStreamBuffer, 0, sizeof(sample_t) * SAMPLE_GRANULARITY * mChannels);
		int sample_count = render_vgmstream2(mStreamBuffer, aSamplesToRead, mParent->mStream);

		for (int j = 0; j < sample_count; j++)
		{
			for (unsigned int k = 0; k < mChannels; k++)
			{
				aBuffer[k * aSamplesToRead + j] = mStreamBuffer[(j * mChannels) + k] / (float)INT16_MAX;
			}
		}

		mOffset += sample_count;

		// If the song is looping, recalculate the offset correctly
		if ((mFlags & AudioSourceInstance::LOOPING) && mOffset >= mParent->mLoopEndSample) {
			mOffset = mOffset - mParent->mLoopEndSample + mParent->mStream->loop_start_sample;
		}

		return sample_count;
	}

	result VGMStreamInstance::rewind()
	{
		reset_vgmstream(mParent->mStream);

		mOffset = 0;
		mStreamPosition = 0.0f;
		return SO_NO_ERROR;
	}

	result VGMStreamInstance::seek(double aSeconds, float* mScratch, unsigned int mScratchSize)
	{
		mOffset = int(floor(mSamplerate * aSeconds));

		seek_vgmstream(mParent->mStream, mOffset);

		mStreamPosition = aSeconds;
		return SO_NO_ERROR;
	}

	bool VGMStreamInstance::hasEnded()
	{
		return !(mFlags & AudioSourceInstance::LOOPING) && mOffset >= mParent->mSampleCount;
	}

	VGMStream::VGMStream() : mStream(nullptr), mSampleCount(0)
	{
	}

	VGMStream::~VGMStream()
	{
		stop();

		if (mStream != nullptr) {
			close_vgmstream(mStream);
		}
	}

	VGMSTREAM* VGMStream::init_vgmstream_with_extension(const char* aFilename, const char* ext)
	{
		STREAMFILE* streamFile = open_stdio_streamfile(aFilename);
		if (streamFile == nullptr) {
			return nullptr;
		}
		// Force extension
		streamFile = open_fakename_streamfile_f(streamFile, nullptr, ext);
		if (streamFile == nullptr) {
			return nullptr;
		}
		VGMSTREAM* stream = init_vgmstream_from_STREAMFILE(streamFile);
		close_streamfile(streamFile);
		return stream;
	}

	result VGMStream::load(const char* aFilename, const char* ext)
	{
		mBaseSamplerate = 0;
		STREAMFILE* zzz_stream = nullptr;

		if (strncmp(aFilename, "zzz://", 6) == 0) {
			zzz_stream = open_ZZZ_STREAMFILE(aFilename + 6);
			if (zzz_stream == nullptr) {
				return FILE_NOT_FOUND;
			}
		} else if (! fileExists(aFilename)) {
			return FILE_NOT_FOUND;
		} else {
			return INVALID_PARAMETER;
		}

		stop();

		if (zzz_stream != nullptr) {
			mStream = init_vgmstream_from_STREAMFILE(zzz_stream);
			close_streamfile(zzz_stream);
		}
		else if (ext != nullptr && ext[0] != '\0') {
			mStream = init_vgmstream_with_extension(aFilename, ext);
		}
		else {
			mStream = init_vgmstream(aFilename);
		}

		if (mStream == nullptr) {
			return FILE_LOAD_FAILED;
		}

		mBaseSamplerate = (float)mStream->sample_rate;
		mSampleCount = (unsigned int)mStream->num_samples;
		mChannels = mStream->channels;

		// Autodetect looping from the file itself and just inform SoLoud about it
		if (mStream->loop_flag) setLooping(true);
		// If the file has no loop tags, but the users wants to loop, force a basic start to end loop
		else if (mFlags & AudioSourceInstance::LOOPING) vgmstream_force_loop(mStream, true, 0, mStream->num_samples);

		mLoopEndSample = mStream->loop_end_sample != 0 ? mStream->loop_end_sample : mSampleCount;

		return SO_NO_ERROR;
	}

	AudioSourceInstance* VGMStream::createInstance()
	{
		return new VGMStreamInstance(this);
	}

	double VGMStream::getLength()
	{
		if (mBaseSamplerate == 0)
			return 0;

		return mSampleCount / mBaseSamplerate;
	}
};
