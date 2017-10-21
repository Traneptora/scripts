/*******************************************************************************
	tbz-crc32.h by thebombzen
*******************************************************************************/

#include <stdio.h>
#include <inttypes.h>

#ifndef TBZ_CRC32_H_
#define TBZ_CRC32_H_

#if defined(_WIN32)
    #if defined(SHARED_EXPORTS)
        #define TBZ_EXPORT __declspec(dllexport)
    #else
        #define TBZ_EXPORT __declspec(dllimport)
    #endif
#elif defined(__GNUC__)
    #if defined(SHARED_EXPORTS)
        #define TBZ_EXPORT __attribute__((visibility("default")))
    #else
        #define TBZ_EXPORT
    #endif
#else
	#define TBZ_EXPORT
#endif

TBZ_EXPORT int tbz_compute_stream_crc32(FILE *input_f, uint32_t *returned_crc, char **error);
TBZ_EXPORT int tbz_compute_file_crc32(const char* filename, uint32_t *returned_crc, char **error);

#endif // TBZ_CRC32_H_
