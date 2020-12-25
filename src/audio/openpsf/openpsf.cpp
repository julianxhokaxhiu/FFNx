/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2020 Julian Xhokaxhiu                                   //
//    Copyright (C) 2020 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
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

#include "openpsf.h"

#include <sys/stat.h>

constexpr auto SOLOUD_OPENPSF_NUM_SAMPLES = 512;
constexpr auto SOLOUD_OPENPSF_VOLUME_SCALE = float(3.2f / double(0x8000));

namespace SoLoud
{
	OpenPsfInstance::OpenPsfInstance(OpenPsf* aParent)
	{
		ended = false;
		mParent = aParent;
		mOffset = 0;
		mStreamBufferSize = SOLOUD_OPENPSF_NUM_SAMPLES * aParent->mChannels;
		mStreamBuffer = new int16_t[mStreamBufferSize];
		mStreamBufferSize *= sizeof(int16_t);
	}

	OpenPsfInstance::~OpenPsfInstance()
	{
		delete[] mStreamBuffer;
	}

	unsigned int OpenPsfInstance::getAudio(float* aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize)
	{
		unsigned int offset = 0;
		unsigned int i, j, k;

		for (i = 0; i < aSamplesToRead; i += SOLOUD_OPENPSF_NUM_SAMPLES)
		{
			memset(mStreamBuffer, 0, mStreamBufferSize);
			unsigned int blockSize = aSamplesToRead - i > SOLOUD_OPENPSF_NUM_SAMPLES ? SOLOUD_OPENPSF_NUM_SAMPLES : aSamplesToRead - i;
			int r = mParent->stream->decode(mStreamBuffer, blockSize);

			if (r == 0) {
				ended = true;
				break;
			}
			else if (r < 0) {
				error("OpenPsfInstance::%s Decoding error: %s\n%s\n", __func__, mParent->stream->get_last_error(), mParent->stream->get_last_status());
				break;
			}

			offset += r;

			for (j = 0; j < blockSize; j++)
			{
				for (k = 0; k < mChannels; k++)
				{
					aBuffer[k * aSamplesToRead + i + j] = mStreamBuffer[j * mChannels + k] * SOLOUD_OPENPSF_VOLUME_SCALE;
				}
			}
		}

		mOffset += offset;

		return offset;
	}

	result OpenPsfInstance::rewind()
	{
		mParent->stream->rewind();

		ended = false;
		mOffset = 0;
		mStreamPosition = 0.0f;
		return SO_NO_ERROR;
	}

	bool OpenPsfInstance::hasEnded()
	{
		return !(mFlags & AudioSourceInstance::LOOPING) && (ended || mOffset >= mParent->mSampleCount);
	}

	OpenPsf::OpenPsf() :
		stream(nullptr), mSampleCount(0)
	{
	}

	OpenPsf::~OpenPsf()
	{
		stop();

		if (stream != nullptr) {
			delete stream;
		}
	}

	bool OpenPsf::is_our_path(const char* aFilename)
	{
		const char* ext = strrchr(aFilename, '.');
		if (!ext) {
			return true;
		}
		return Psf::is_our_path(aFilename, ext + 1);
	}

	result OpenPsf::load(const char* aFilename)
	{
		mBaseSamplerate = 0;

		if (aFilename == nullptr) {
			return INVALID_PARAMETER;
		}

		struct stat dummy;
		if (stat(aFilename, &dummy) != 0) {
			return FILE_NOT_FOUND;
		}

		stop();

		stream = new Psf(PsfFlags::PsfDefaults, 0);
		if (!stream->open(aFilename, true)) {
			error("Cannot open file %s: %s\n%s\n", aFilename, stream->get_last_error(), stream->get_last_status());
			return FILE_LOAD_FAILED;
		}

		mBaseSamplerate = float(stream->get_sample_rate());
		mSampleCount = stream->get_sample_count();
		mChannels = stream->get_channel_count();
		setLooping(true);

		info("Opening file %s with openPSF (samplerate: %i, samplecount: %i, channels: %i):\n%s\n",
			aFilename, int(mBaseSamplerate), stream->get_sample_count(), mChannels, stream->get_last_status());

		return SO_NO_ERROR;
	}

	AudioSourceInstance* OpenPsf::createInstance()
	{
		return new OpenPsfInstance(this);
	}

	double OpenPsf::getLength()
	{
		if (mBaseSamplerate == 0) {
			return 0;
		}

		return mSampleCount / mBaseSamplerate;
	}
};
