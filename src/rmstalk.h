#ifndef RMSTALK_H
#define RMSTALK_H

#include <stdio.h>
#include <stdbool.h>
#include <portaudio.h>
#include <vorbis/vorbisenc.h>


#define SAMPLE_RATE 44100


typedef struct
{
	FILE *debug_output;
	
	vorbis_info vi;
	vorbis_comment vc;
	vorbis_dsp_state v;
	vorbis_block vb;
	ogg_stream_state os;
	
	PaStream *input_stream;
} conn_state_t;


/****** rmstalk.c ******/

void check(bool success, const char *err_format, ...);
void send_data(conn_state_t *state, const unsigned char *data, long len);


/****** audio.c ******/

void rms_audio_init();
void rms_audio_destroy();
void rms_audio_input_start(conn_state_t *state);
void rms_audio_input_stop(conn_state_t *state);


/****** vorbis.c ******/

void rms_vorbis_init(conn_state_t *state);
void rms_vorbis_destroy(conn_state_t *state);

/** Appends floating point samples to the uncompressed vorbis buffer
*/
void rms_vorbis_buffer_write(conn_state_t *state, const float *buffer, int len);

/** Analyses and compresses the vorbis buffer to ogg pages, which are
	written with rms_ogg_page_send().
*/
void rms_vorbis_buffer_flush(conn_state_t *state);

/** Writes the eos page to the output
*/
void rms_vorbis_buffer_close(conn_state_t *state);


void rms_ogg_write_headers(conn_state_t *state);

#endif