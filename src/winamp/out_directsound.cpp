#include "out_directsound.h"

#define AUDIO_BUFFER_SIZE 5

CustomOutPluginState CustomOutPlugin::state = CustomOutPluginState();
CustomOutPluginState CustomOutPlugin::dup_state = CustomOutPluginState();
WinampOutContext CustomOutPlugin::static_context = WinampOutContext();
int CustomOutPlugin::last_pause = 0;
int CustomOutPlugin::last_volume = 255;

CustomOutPlugin::CustomOutPlugin() :
	AbstractOutPlugin()
{
	mod = new WinampOutModule();
	mod->version = OUT_VER;
	mod->description = "FFNx Winamp Output Plugin";
	mod->id = 38; // DirectSound id
	mod->hMainWindow = nullptr;
	mod->hDllInstance = nullptr;
	mod->Config = FakeDialog;
	mod->About = FakeDialog;
	mod->Init = Noop;
	mod->Quit = Noop;
	mod->Open = Open;
	mod->Close = Close;
	mod->Write = Write;
	mod->CanWrite = CanWrite;
	mod->IsPlaying = IsPlaying;
	mod->Pause = Pause;
	mod->SetVolume = SetVolume;
	mod->SetPan = SetPan;
	mod->Flush = Flush;
	mod->GetOutputTime = GetOutputTime;
	mod->GetWrittenTime = GetWrittenTime;

	static_context.version = OUT_CONTEXT_VER;
	static_context.Duplicate = Duplicate;
	static_context.Resume = Resume;
	static_context.CancelDuplicate = CancelDuplicate;

	context = &static_context;
}

CustomOutPlugin::~CustomOutPlugin()
{
	delete mod;
}

void CustomOutPlugin::FakeDialog(HWND hwndParent)
{
	UNUSED_PARAM(hwndParent);
	// Nothing to do
}

void CustomOutPlugin::Noop()
{
	// Nothing to do
}

int CustomOutPlugin::Open(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms)
{
	UNUSED_PARAM(bufferlenms);
	UNUSED_PARAM(prebufferms);

	if (trace_all || trace_music) trace("Open directsound %i %i %i\n", samplerate, numchannels, bitspersamp);

	Close();

	DSBUFFERDESC1 sbdesc = DSBUFFERDESC1();

	state.sound_format.cbSize = 0;
	state.sound_format.wBitsPerSample = bitspersamp;
	state.sound_format.nChannels = numchannels;
	state.sound_format.nSamplesPerSec = samplerate;
	state.sound_format.nBlockAlign = state.sound_format.nChannels * state.sound_format.wBitsPerSample / 8;
	state.sound_format.nAvgBytesPerSec = state.sound_format.nSamplesPerSec * state.sound_format.nBlockAlign;
	state.sound_format.wFormatTag = WAVE_FORMAT_PCM;

	state.sound_buffer_size = state.sound_format.nAvgBytesPerSec * AUDIO_BUFFER_SIZE;
	state.prebuffer_size = state.sound_format.nAvgBytesPerSec / 10; // ~100 ms
	state.sound_write_pointer = 0;
	state.bytes_written = 0;

	state.start_t = 0;
	state.offset_t = 0;
	state.paused_t = 0;
	state.last_stop_t = 0;
	state.last_pause_t = 0;

	state.play_started = false;
	state.clear_done = false;

	sbdesc.dwSize = sizeof(sbdesc);
	sbdesc.lpwfxFormat = &state.sound_format;
	sbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY
		| DSBCAPS_CTRLPAN | DSBCAPS_GETCURRENTPOSITION2
		| DSBCAPS_TRUEPLAYPOSITION | DSBCAPS_GLOBALFOCUS;
	sbdesc.dwReserved = 0;
	sbdesc.dwBufferBytes = state.sound_buffer_size;

	if (!*common_externals.directsound)
	{
		error("No directsound device\n");

		return -1;
	}

	if ((*common_externals.directsound)->CreateSoundBuffer((LPCDSBUFFERDESC)&sbdesc, &state.sound_buffer, 0))
	{
		error("couldn't create sound buffer (%i, %i)\n", numchannels, samplerate);
		state.sound_buffer = nullptr;

		return -1;
	}

	SetVolume(-1); // Set last known volume to buffer

	return 0;
}

void CustomOutPlugin::Close()
{
	if (trace_all || trace_music) trace("Close directsound (%i)\n", *common_externals.directsound ? 1 : 0);

	if (state.sound_buffer) {
		state.last_stop_t = GetTickCount();
		if (*common_externals.directsound && (state.sound_buffer->Stop() || state.sound_buffer->Release())) {
			error("couldn't stop or release sound buffer (%i)\n", GetLastError());
		}
		state.sound_buffer = nullptr;
	}
}

bool CustomOutPlugin::playHelper()
{
	if (trace_all || trace_music) trace("Play directsound\n");

	if (state.sound_buffer->Play(0, 0, DSBPLAY_LOOPING)) {
		error("couldn't play sound buffer\n");
		return false;
	}

	state.start_t = GetTickCount();
	state.play_started = true;

	return true;
}

int CustomOutPlugin::Write(char* buffer, int len)
{
	LPVOID ptr1, ptr2;
	DWORD bytes1, bytes2;

	state.clear_done = false;

	if (!state.sound_buffer || !*common_externals.directsound) {
		return 1;
	}

	if (state.sound_buffer->Lock(state.sound_write_pointer, len, &ptr1, &bytes1, &ptr2, &bytes2, 0)) {
		error("couldn't lock sound buffer\n");
		return 1;
	}

	memcpy(ptr1, buffer, bytes1);
	memcpy(ptr2, buffer + bytes1, bytes2);

	if (state.sound_buffer->Unlock(ptr1, bytes1, ptr2, bytes2)) {
		error("couldn't unlock sound buffer\n");
		return 1;
	}

	state.sound_write_pointer = (state.sound_write_pointer + bytes1 + bytes2) % state.sound_buffer_size;
	state.bytes_written += bytes1 + bytes2;

	//trace("Write bytes_written=%i sound_write_pointer=%i (sound_buffer_size=%i) len=%i bytes1=%i bytes2=%i\n", bytes_written, sound_write_pointer, sound_buffer_size, len, bytes1, bytes2);

	// Do not play the song until buffering reach 'prebuffer_size' value
	if (!last_pause && !state.play_started && state.bytes_written >= state.prebuffer_size
		&& !playHelper()) {
		return 1;
	}

	return 0;
}

int CustomOutPlugin::canWriteHelper(DWORD& current_play_cursor)
{
	state.sound_buffer->GetCurrentPosition(&current_play_cursor, nullptr);

	//trace("Can Write current_play_cursor=%i\n", current_play_cursor);

	if (state.play_started && current_play_cursor == state.sound_write_pointer) {
		return 0;
	}

	if (current_play_cursor <= state.sound_write_pointer) {
		return (state.sound_buffer_size - state.sound_write_pointer) + current_play_cursor;
	}
	return current_play_cursor - state.sound_write_pointer;
}

int CustomOutPlugin::CanWrite()
{
	if (!state.sound_buffer || !*common_externals.directsound) {
		return 0;
	}

	DWORD current_play_cursor;
	return canWriteHelper(current_play_cursor);
}

int CustomOutPlugin::IsPlaying()
{
	if (last_pause || !state.sound_buffer || !*common_externals.directsound) {
		return 0;
	}

	// IsPlaying is called at the very end of decoding,
	// if we never started the song we should do it now
	// and we should deactivate the loop flag
	if (!state.play_started && !playHelper()) {
		return 0;
	}

	DWORD current_play_cursor;
	const int can_write = canWriteHelper(current_play_cursor);
	const int clear_data_size = state.sound_buffer_size / AUDIO_BUFFER_SIZE / 2; // ~500 ms

	// It is hard to know the current accurate play time with DirectSound
	// And the buffer is circular, so we make precautions
	if (state.clear_done) {
		if (current_play_cursor < state.sound_write_pointer
			|| current_play_cursor > state.sound_write_pointer + clear_data_size) {
			return 1;
		}

		Close();

		return 0;
	}

	// Clear some data after the write pointer when it is possible (to remove artifacts)
	if (can_write >= clear_data_size) {
		LPVOID ptr1, ptr2;
		DWORD bytes1, bytes2;

		if (!state.sound_buffer->Lock(state.sound_write_pointer, clear_data_size, &ptr1, &bytes1, &ptr2, &bytes2, 0)) {
			memset(ptr1, 0, bytes1);
			memset(ptr2, 0, bytes2);

			if (state.sound_buffer->Unlock(ptr1, bytes1, ptr2, bytes2)) {
				error("couldn't unlock sound buffer\n");
			}
		}
		else {
			error("couldn't lock sound buffer\n");
		}

		state.clear_done = true;
	}

	return 1;
}

int CustomOutPlugin::Pause(int pause)
{
	int t = last_pause;

	if (last_pause != pause) {
		if (pause) { // Pause
			state.last_pause_t = GetTickCount();

			if (state.sound_buffer && *common_externals.directsound && state.play_started) {
				if (trace_all || trace_music) trace("Stop directsound\n");

				if (state.sound_buffer->Stop()) {
					error("couldn't stop sound buffer\n");
				}
			}
		}
		else {
			if (state.last_pause_t > 0) { // UnPause
				state.paused_t += GetTickCount() - state.last_pause_t; // Remove paused time
				state.last_pause_t = 0;
			}

			if (state.sound_buffer && *common_externals.directsound) {
				if (trace_all || trace_music) trace("Play directsound\n");

				if ((!state.play_started && state.bytes_written >= state.prebuffer_size && !playHelper())
					|| state.sound_buffer->Play(0, 0, DSBPLAY_LOOPING)) {
					error("couldn't play sound buffer\n");
				}
			}
		}
	}

	last_pause = pause;

	return t;
}

void CustomOutPlugin::SetVolume(int volume)
{
	if (volume < 0 || volume > 255) {
		volume = last_volume;
	}
	else {
		last_volume = volume;
	}

	if (state.sound_buffer && *common_externals.directsound) {
		float decibel = DSBVOLUME_MIN;
		if (volume != 0) {
			decibel = 2000.0f * log10f(volume / 255.0f);
		}

		if (trace_all || trace_music) trace("Set directsound attenuation %f Db\n", decibel);

		state.sound_buffer->SetVolume(decibel);
	}
}

void CustomOutPlugin::SetPan(int pan)
{
	if (state.sound_buffer && *common_externals.directsound) {
		if (trace_all || trace_music) trace("Set pan %i\n", pan);

		state.sound_buffer->SetPan(pan * DSBPAN_LEFT / 128);
	}
}

void CustomOutPlugin::Flush(int t)
{
	if (trace_all || trace_music) trace("Directsound flush buffer, seek to %i\n", t);

	if (state.sound_buffer && *common_externals.directsound) {
		if (!last_pause && state.play_started) {
			state.sound_buffer->Stop();
		}

		LPVOID ptr1, ptr2;
		DWORD bytes1, bytes2;

		// Clear the entire buffer
		if (!state.sound_buffer->Lock(0, 0, &ptr1, &bytes1, &ptr2, &bytes2, DSBLOCK_ENTIREBUFFER)) {
			memset(ptr1, 0, bytes1);
			memset(ptr2, 0, bytes2);

			if (state.sound_buffer->Unlock(ptr1, bytes1, ptr2, bytes2)) {
				error("couldn't unlock sound buffer\n");
			}
		}
		else {
			error("couldn't lock sound buffer\n");
		}

		state.sound_write_pointer = 0;
		state.bytes_written = 0;
		state.sound_buffer->SetCurrentPosition(0);
		state.play_started = false;
	}

	state.last_pause_t = 0;
	state.last_stop_t = 0;
	state.paused_t = 0;
	state.offset_t = t;
}

int CustomOutPlugin::GetOutputTime()
{
	DWORD t;

	if (state.last_pause_t > 0) {
		t = state.last_pause_t;
	}
	else if (state.last_stop_t > 0) {
		t = state.last_stop_t;
	}
	else if (state.play_started) {
		t = GetTickCount();
	}
	else {
		return state.offset_t;
	}

	return state.offset_t + (t - state.start_t) - state.paused_t;
}

int CustomOutPlugin::GetWrittenTime()
{
	if (0 == state.sound_format.nAvgBytesPerSec) {
		return state.offset_t + state.bytes_written;
	}

	int written_time = state.bytes_written / state.sound_format.nAvgBytesPerSec * 1000;
	written_time += ((state.bytes_written % state.sound_format.nAvgBytesPerSec) * 1000) / state.sound_format.nAvgBytesPerSec;

	return state.offset_t + written_time;
}

void CustomOutPlugin::Duplicate()
{
	CancelDuplicate();

	if (nullptr == state.sound_buffer) {
		return;
	}

	if (state.sound_buffer && *common_externals.directsound
		&& state.play_started && state.sound_buffer->Stop()) {
		error("couldn't stop sound buffer\n");
	}

	// Save current state
	memcpy(&dup_state, &state, sizeof(state));

	state.sound_buffer = nullptr;
}

int CustomOutPlugin::Resume(int samplerate, int numchannels, int bitspersamp, int bufferlenms, int prebufferms)
{
	if (nullptr != state.sound_buffer) {
		Close();
	}

	if (nullptr == dup_state.sound_buffer) {
		return Open(samplerate, numchannels, bitspersamp, bufferlenms, prebufferms);
	}

	// Restore saved state
	memcpy(&state, &dup_state, sizeof(state));

	dup_state.sound_buffer = nullptr;

	SetVolume(-1); // Set last known volume to buffer

	if (state.play_started && state.sound_buffer->Play(0, 0, DSBPLAY_LOOPING)) {
		error("couldn't play sound buffer\n");
	}

	return 0;
}

int CustomOutPlugin::CancelDuplicate()
{
	if (nullptr == dup_state.sound_buffer) {
		return 0;
	}

	if (*common_externals.directsound && (dup_state.sound_buffer->Stop() || dup_state.sound_buffer->Release())) {
		error("CancelDuplicate: couldn't stop or release sound buffer (%i)\n", GetLastError());
	}

	dup_state.sound_buffer = nullptr;

	return 1;
}

void CustomOutPlugin::setTempo(int tempo)
{
	if (state.sound_buffer && *common_externals.directsound) {
		if (trace_all || trace_music) trace("Set tempo %i\n", tempo);

		state.sound_buffer->SetFrequency((state.sound_format.nSamplesPerSec * (tempo + 480)) / 512);
	}
}
