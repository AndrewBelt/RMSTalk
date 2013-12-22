#include <stdlib.h>
#include <stdarg.h>
#include "rmstalk.h"


void check(bool success, const char *err_format, ...)
{
	va_list vl;
	va_start(vl, err_format);
	
	if (!success)
	{
		if (err_format)
		{
			vfprintf(stderr, err_format, vl);
			fprintf(stderr, "\n");
		}
		abort();
	}
	va_end(vl);
}


void send_data(conn_state_t *state, const unsigned char *data, long len)
{
	// TODO
	// This is where the data should be sent over the network.
	fwrite(data, 1, len, state->debug_output);
	fflush(state->debug_output);
}
