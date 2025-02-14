/**
 * @file tranep-crc32.h
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

#ifndef TRANEP_CRC32_H_
#define TRANEP_CRC32_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#if defined(_WIN32)
    #if defined(SHARED_EXPORTS)
        #define TRANEP_EXPORT __declspec(dllexport)
    #else
        #define TRANEP_EXPORT __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__CLANG__)
    #if defined(SHARED_EXPORTS)
        #define TRANEP_EXPORT __attribute__((visibility("default")))
    #else
        #define TRANEP_EXPORT __attribute__((visibility("hidden")))
    #endif
#else
    #define TRANEP_EXPORT
#endif

/**
 * Compute the CRC32 of a FILE* stream.
 * @param input The stream for which we want to compute the crc32.
 * @param returned_crc The calculated crc32 will be stored inside this pointer. Left untouched upon failure.
 * @param error The error string, will be stored inside this pointer. Left unchanged upon success.
 * @return Zero upon success, nonzero upon failure.
 */
TRANEP_EXPORT int tranep_compute_stream_crc32(FILE *input_f, uint32_t *crc, const char **error);

/**
 * Compute the CRC32 of an in-memory buffer.
 * 
 * @param buffer The buffer for which we want to compute the crc32.
 * @param buflen The size of the buffer.
 * @return The calculated crc32
 */
TRANEP_EXPORT uint32_t tranep_compute_buffer_crc32(const uint8_t *buffer, size_t buflen);

/**
 * Compute the CRC32 of a file located on the filesystem.
 * 
 * @param filename The path to the file for which we want to compute the crc32.
 * @param crc The calculated crc32 will be stored inside this pointer. Left untouched upon failure.
 * @param error The error string, will be stored inside this pointer. Left unchanged upon success.
 * @return Zero upon success, nonzero upon failure.
 */
TRANEP_EXPORT int tranep_compute_filename_crc32(const char *filename, uint32_t *crc, const char **error);

#endif // TRANEP_CRC32_H_
