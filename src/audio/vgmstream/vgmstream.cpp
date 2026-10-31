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

#include <algorithm>
#include <memory>
#include <vector>

namespace
{
	struct WavMemoryFile : STREAMFILE
	{
		std::shared_ptr<const std::vector<uint8_t>> data;
		offv_t offset = 0;
	};

	STREAMFILE* openWavMemoryFile(std::shared_ptr<const std::vector<uint8_t>> data)
	{
		auto file = new WavMemoryFile{};
		file->data = std::move(data);
		file->read = [](STREAMFILE* stream, uint8_t* dst, offv_t offset, size_t length) -> size_t
		{
			auto file = static_cast<WavMemoryFile*>(stream);
			if (offset < 0 || uint64_t(offset) >= file->data->size()) return 0;
			length = std::min(length, file->data->size() - size_t(offset));
			memcpy(dst, file->data->data() + size_t(offset), length);
			file->offset = offset + length;
			return length;
		};
		file->get_size = [](STREAMFILE* stream) { return static_cast<WavMemoryFile*>(stream)->data->size(); };
		file->get_offset = [](STREAMFILE* stream) { return static_cast<WavMemoryFile*>(stream)->offset; };
		file->get_name = [](STREAMFILE*, char* name, size_t size) { snprintf(name, size, "native.wav"); };
		file->open = [](STREAMFILE* stream, const char* name, size_t) -> STREAMFILE*
		{
			if (strcmp(name, "native.wav") != 0) return nullptr;
			return openWavMemoryFile(static_cast<WavMemoryFile*>(stream)->data);
		};
		file->close = [](STREAMFILE* stream) { delete static_cast<WavMemoryFile*>(stream); };
		return file;
	}

	void appendWavUint(std::vector<uint8_t>& data, uint32_t value)
	{
		for (int shift = 0; shift < 32; shift += 8) data.push_back(uint8_t(value >> shift));
	}
}

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

		return finishLoad();
	}

	result VGMStream::loadWavFromArchive(const char* filename, uint32_t offset, uint32_t length,
		std::span<const uint8_t> format, uint32_t loopStart, uint32_t loopEnd)
	{
		STREAMFILE* archive = open_stdio_streamfile(filename);
		if (archive == nullptr) return FILE_NOT_FOUND;

		const size_t archiveSize = get_streamfile_size(archive);
		const uint64_t wavSize = 12ull + 8 + format.size() + (format.size() & 1) + 8 + length + (length & 1);
		if (length == 0 || format.size() < 16 || uint64_t(offset) + length > archiveSize || wavSize > UINT32_MAX)
		{
			close_streamfile(archive);
			return INVALID_PARAMETER;
		}

		auto data = std::make_shared<std::vector<uint8_t>>();
		data->reserve(size_t(wavSize));
		data->insert(data->end(), {'R', 'I', 'F', 'F'});
		appendWavUint(*data, uint32_t(wavSize - 8));
		data->insert(data->end(), {'W', 'A', 'V', 'E', 'f', 'm', 't', ' '});
		appendWavUint(*data, uint32_t(format.size()));
		data->insert(data->end(), format.begin(), format.end());
		if (format.size() & 1) data->push_back(0);
		data->insert(data->end(), {'d', 'a', 't', 'a'});
		appendWavUint(*data, length);
		const size_t payloadOffset = data->size();
		data->resize(size_t(wavSize), 0);
		const size_t bytesRead = read_streamfile(data->data() + payloadOffset, offset, length, archive);
		close_streamfile(archive);
		if (bytesRead != length) return FILE_LOAD_FAILED;

		stop();
		if (mStream != nullptr) close_vgmstream(mStream);
		STREAMFILE* wave = openWavMemoryFile(std::move(data));
		mStream = init_vgmstream_from_STREAMFILE(wave);
		close_streamfile(wave);
		if (mStream == nullptr) return FILE_LOAD_FAILED;

		if ((mFlags & AudioSourceInstance::LOOPING) && loopEnd != 0)
		{
			if (loopStart >= loopEnd || loopEnd > uint32_t(mStream->num_samples))
			{
				close_vgmstream(mStream);
				mStream = nullptr;
				return INVALID_PARAMETER;
			}
			vgmstream_force_loop(mStream, true, loopStart, loopEnd);
		}
		return finishLoad();
	}

	result VGMStream::finishLoad()
	{
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
