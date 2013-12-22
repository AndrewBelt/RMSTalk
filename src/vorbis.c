#include <stdlib.h>
#include "rmstalk.h"


void rms_ogg_page_send(conn_state_t *state, ogg_page *og);



void rms_vorbis_init(conn_state_t *state)
{
	int err;
	
	vorbis_info_init(&state->vi);
	err = vorbis_encode_init_vbr(&state->vi, 2, SAMPLE_RATE, 0.1);
	check(err == 0, "Could not set vorbis quality mode");
	
	vorbis_comment_init(&state->vc);
	vorbis_analysis_init(&state->v, &state->vi);
	vorbis_block_init(&state->v, &state->vb);
	ogg_stream_init(&state->os, rand());
	
}

void rms_vorbis_destroy(conn_state_t *state)
{
	ogg_stream_clear(&state->os);
	vorbis_block_clear(&state->vb);
	vorbis_dsp_clear(&state->v);
	vorbis_comment_clear(&state->vc);
	vorbis_info_clear(&state->vi);
}


void rms_vorbis_buffer_write(conn_state_t *state, const float *buffer, int len)
{
	float **out = vorbis_analysis_buffer(&state->v, len);
	
	// Copy the input buffer to the output buffer
	for (int i = 0; i < len; i++)
	{
		float val = buffer[i];
		out[0][i] = val;
	}
	
	vorbis_analysis_wrote(&state->v, len);
}


void rms_vorbis_buffer_flush(conn_state_t *state)
{
	while (true)
	{
		int ret;
		ret = vorbis_analysis_blockout(&state->v, &state->vb);
		check(ret >= 0, "vorbis_analysis_blockout() failed");
		
		if (ret == 0)
			break;
		
		// We're using VBR, so we can't generate an ogg_packet here yet
		vorbis_analysis(&state->vb, NULL);
		vorbis_bitrate_addblock(&state->vb);
		
		ogg_packet op;
		while (vorbis_bitrate_flushpacket(&state->v, &op))
		{
			ogg_stream_packetin(&state->os, &op);
			
			while (true)
			{
				ogg_page og;
				int ret = ogg_stream_pageout(&state->os, &og);
				
				if (ret == 0)
					break;
				
				rms_ogg_page_send(state, &og);
				
				// wut
				if (ogg_page_eos(&og))
					break;
			}
		}
	}
}

void rms_vorbis_buffer_close(conn_state_t *state)
{
	vorbis_analysis_wrote(&state->v, 0);
	rms_vorbis_buffer_flush(state);
}

void rms_ogg_write_headers(conn_state_t *state)
{
	ogg_packet op;
	ogg_packet op_comm;
	ogg_packet op_code;
	
	vorbis_analysis_headerout(&state->v, &state->vc, &op, &op_comm, &op_code);
	ogg_stream_packetin(&state->os, &op);
	ogg_stream_packetin(&state->os, &op_comm);
	ogg_stream_packetin(&state->os, &op_code);
	
	while (true)
	{
		ogg_page og;
		
		// Regardless of our position in the page, flush and send it
		int ret = ogg_stream_flush(&state->os, &og);
		if (ret == 0)
			break;
		
		rms_ogg_page_send(state, &og);
	}
}


void rms_ogg_page_send(conn_state_t *state, ogg_page *og)
{
	send_data(state, og->header, og->header_len);
	send_data(state, og->body, og->body_len);
}
