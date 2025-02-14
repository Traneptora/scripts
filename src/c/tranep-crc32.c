/**
 * @file tranep-crc32.c
 * @author Leo Izen (Traneptora) <leo.izen@gmail.com>
 *
 * This file is in the public domain unless this is not possible by law.
 * In that case, you may do anything you want with this file for any
 * reason with or without permission.
 *
 * This file is provided as-is. All warranties are waivied, implied or
 * otherwise, including but not limited to the implied warranties of
 * merchantability and fitness for a particular purpose.
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>
#include "tranep-crc32.h"

TRANEP_EXPORT int tranep_compute_filename_crc32(const char *filename, uint32_t *crc, const char **error) {
    FILE *input;
    int status;

    input = fopen(filename, "rb");
    if(!input) {
        *error = strerror(errno);
        return 2;
    }
    status = tranep_compute_stream_crc32(input, crc, error);
    fclose(input);

    return status;
}

TRANEP_EXPORT int tranep_compute_stream_crc32(FILE *input, uint32_t *crc, const char **error){
    size_t chunk_size = 1 << 12;
    uint8_t *buffer;
    uint32_t calc_crc = 0;
    size_t bytes_read;

    buffer = malloc(chunk_size);

    if(!buffer) {
        *error = strerror(ENOMEM);
        return 1;
    }

    while (1) {

        bytes_read = fread(buffer, 1, chunk_size, input);

        if (bytes_read > 0)
            calc_crc = crc32_z(calc_crc, buffer, bytes_read);

        if (bytes_read < chunk_size) {
            if (ferror(input)) {
                *error = strerror(errno);
                free(buffer);
                return 1;
            } else if (feof(input)) {
                 break;
            }
        }

    }

    free(buffer);
    *crc = calc_crc;

    return 0;
}

TRANEP_EXPORT uint32_t tranep_compute_buffer_crc32(const uint8_t *buffer, size_t buflen) {
    return crc32_z(0, buffer, buflen);
}
