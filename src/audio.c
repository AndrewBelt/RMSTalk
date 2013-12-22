#include "rmstalk.h"


int rms_audio_input_callback(const void *in, void *out,
	unsigned long frames, const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags flags, void *data)
{
	conn_state_t *state = data;
	rms_vorbis_buffer_write(state, (const float *) in, frames);
	
	// TODO
	// We don't actually want to flush the data here because we're
	// in the audio thread.
	rms_vorbis_buffer_flush(state);
	
	return 0;
}

void rms_audio_init()
{
	PaError err = Pa_Initialize();
	check(err == paNoError, "Could not initialize PortAudio");
}

void rms_audio_destroy()
{
	Pa_Terminate();
}

void rms_audio_input_start(conn_state_t *state)
{
	PaError err;
	err = Pa_OpenDefaultStream(&state->input_stream, 1, 0,
		paFloat32, SAMPLE_RATE, paFramesPerBufferUnspecified,
		rms_audio_input_callback, state);
	check(err == paNoError, "Could not open input stream");
	
	err = Pa_StartStream(state->input_stream);
	check(err == paNoError, "Could not start input stream");
}

void rms_audio_input_stop(conn_state_t *state)
{
	Pa_StopStream(state->input_stream);
	Pa_CloseStream(state->input_stream);
}

