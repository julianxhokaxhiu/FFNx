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

#include "winamp.h"

constexpr auto SOLOUD_WINAMP_CHUNK_SAMPLES = 512;

namespace SoLoud
{
	WinampInstance::WinampInstance(Winamp* aParent) :
		mParent(aParent), mOffset(0)
	{
		mBytesPerSample = 2 * mParent->outPlugin->numChannels();
		mAudioBufferSize = SOLOUD_WINAMP_CHUNK_SAMPLES * mBytesPerSample;
		mAudioBuffer = new char[mAudioBufferSize];
	}

	WinampInstance::~WinampInstance()
	{
		delete[] mAudioBuffer;
	}

	unsigned int WinampInstance::getAudio(float* aBuffer, unsigned int aSamplesToRead, unsigned int aBufferSize)
	{
		unsigned int offsetBytes = 0;
		unsigned int i, j, k;
		unsigned int offset;
		int16_t* buf16 = reinterpret_cast<int16_t*>(mAudioBuffer);

		for (i = 0; i < aSamplesToRead; i += SOLOUD_WINAMP_CHUNK_SAMPLES)
		{
			memset(mAudioBuffer, 0, mAudioBufferSize);
			unsigned int blockSize = (aSamplesToRead - i) > SOLOUD_WINAMP_CHUNK_SAMPLES ? SOLOUD_WINAMP_CHUNK_SAMPLES : aSamplesToRead - i;
			offsetBytes += mParent->outPlugin->read(mAudioBuffer, blockSize * mBytesPerSample);

			for (j = 0; j < blockSize; j++)
			{
				for (k = 0; k < mChannels; k++)
				{
					aBuffer[k * aSamplesToRead + i + j] = buf16[j * mChannels + k] / (float)INT16_MAX;
				}
			}
		}

		offset = offsetBytes / mBytesPerSample;

		mOffset += offset;

		return offset;
	}

	result WinampInstance::rewind()
	{
		mParent->inPlugin->setOutputTime(0);

		mOffset = 0;
		mStreamPosition = 0.0f;
		return SO_NO_ERROR;
	}

	result WinampInstance::seek(double aSeconds, float* mScratch, unsigned int mScratchSize)
	{
		mParent->inPlugin->setOutputTime(aSeconds);
		mStreamPosition = aSeconds;
		return SO_NO_ERROR;
	}

	bool WinampInstance::hasEnded()
	{
		return !(mFlags & AudioSourceInstance::LOOPING) && mParent->outPlugin->finishedPlaying();
	}

	float WinampInstance::getInfo(unsigned int aInfoKey)
	{
		// Can resume music
		if (aInfoKey == 7777) {
			// Needs to Duplicate
			return 1.0f;
		}

		return 0.0f;
	}

	Winamp::Winamp(WinampInPlugin* inPlugin, BufferOutPlugin* outPlugin) :
		inPlugin(inPlugin), outPlugin(outPlugin)
	{
	}

	Winamp::~Winamp()
	{
		stop();
		inPlugin->stop();
		outPlugin->getModule()->Close();
	}

	result Winamp::load(const char* aFilename)
	{
		if (aFilename == 0) {
			return INVALID_PARAMETER;
		}

		if (inPlugin->accept(aFilename)) {
			error("This file type is not supported (%s)\n", aFilename);
			return FILE_LOAD_FAILED;
		}

		stop();

		inPlugin->play(aFilename);

		if (outPlugin->sampleRate() < 0) {
			error("The winamp output plugin was never opened\n");
			return FILE_LOAD_FAILED;
		}

		if (outPlugin->bitsPerSample() != 16) {
			error("Unsupported bits per sample value: %i\n", outPlugin->bitsPerSample());
			return FILE_LOAD_FAILED;
		}

		mBaseSamplerate = outPlugin->sampleRate();
		mChannels = outPlugin->numChannels();

		setLooping(true);

		char looping[2];
		if (inPlugin->getTag(aFilename, "looping", looping, 2)
			&& strncmp(looping, "0", 1) == 0) {
			setLooping(false);
		}

		return SO_NO_ERROR;
	}

	AudioSourceInstance* Winamp::createInstance()
	{
		return new WinampInstance(this);
	}

	double Winamp::getLength()
	{
		// Needs to convert from ms to s
		return inPlugin->getLength() / 1000.0;
	}
};
