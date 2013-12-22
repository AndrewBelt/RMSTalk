#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "rmstalk.h"


int main(int argc, char **argv)
{
	srand(time(NULL));
	conn_state_t state;
	state.debug_output = fopen("output.ogg", "wb");
	
	rms_vorbis_init(&state);
	rms_ogg_write_headers(&state);
	
	rms_audio_init();
	printf("Recording...\n");
	rms_audio_input_start(&state);
	Pa_Sleep(10 * 1000);
	rms_audio_input_stop(&state);
	printf("Done!\n");
	rms_audio_destroy();
	
	rms_vorbis_buffer_close(&state);
	fclose(state.debug_output);
	rms_vorbis_destroy(&state);
	
	return 0;
}