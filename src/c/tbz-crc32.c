/*******************************************************************************
	tbz-crc32.c by thebombzen
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>

#include <zlib.h>
#include "tbz-crc32.h"

/**
 * [tbz_compute_file_crc32 Compute the CRC32 of a file located on the filesystem.]
 * @param  filename     [The path to the file for which we want to compute the crc32.]
 * @param  returned_crc [The calculated crc32 will be stored inside this pointer. Left untouched upon failure.]
 * @param  error        [The error string, will be stored inside this pointer. Left unchanged upon success.]
 * @return              [Zero upon success, nonzero upon failure.]
 */

TBZ_EXPORT int tbz_compute_file_crc32(const char* filename, uint32_t *returned_crc, char **error){
	FILE *input_f = fopen(filename, "r");
	if(input_f == NULL) {
		*error = strerror(errno);
		return 2;
	}
	int status = tbz_compute_stream_crc32(input_f, returned_crc, error);
	fclose(input_f);
	return status;
}

/**
 * [tbz_compute_stream_crc32 Compute the CRC32 of a FILE* stream.]
 * @param  input_f      [The stream for which we want to compute the crc32.]
 * @param  returned_crc [The calculated crc32 will be stored inside this pointer. Left untouched upon failure.]
 * @param  error        [The error string, will be stored inside this pointer. Left unchanged upon success.]
 * @return              [Zero upon success, nonzero upon failure.]
 */
TBZ_EXPORT int tbz_compute_stream_crc32(FILE *input_f, uint32_t *returned_crc, char **error){
	size_t chunk_size = 1 << 12;
	uint8_t *buffer;
	uint32_t crc;

	buffer = (uint8_t *) malloc(chunk_size * sizeof(uint8_t));

	if(buffer == NULL) {
		*error = strerror(errno);
		return 1;
	}

	crc = crc32_z(0, Z_NULL, 0);
	
	while(1){

		size_t bytes_read = fread(buffer, 1, chunk_size, input_f);
		
		if (bytes_read > 0){
			crc = crc32_z(crc, buffer, bytes_read);
		}

		if (bytes_read < chunk_size) {
			if (ferror(input_f)){
				*error = strerror(errno);
			 	free(buffer);
			 	return 1;
			} else {
			 	break;
			}
		}

	}

	free(buffer);
 	*returned_crc = crc;
	return 0;
}
