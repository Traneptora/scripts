/**
 * @file crc32.c
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

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "tranep-crc32.h"


int main(int argc, char **argv) {
    uint32_t crc;
    int status;
    int ret = 0;
    const char *error;

    if (argc < 2 || !strncmp("--help", argv[1], 7)){
        fprintf(argc < 2 ? stderr : stdout, "%s filename...\n", argv[0]);
        return argc < 2 ? 1 : 0;
    }

    if (argc == 2) {
        status = tranep_compute_filename_crc32(argv[1], &crc, &error);
        if (!status){
            printf("%08X\n", crc);
        } else {
            fprintf(stderr, "%s: %s: %s\n", argv[0], argv[1], error);
            ret = 2;
        }
    } else {
        for (int i = 1; i < argc; i++){
            status = tranep_compute_filename_crc32(argv[i], &crc, &error);
            if (!status){
                printf("%08X %s\n", crc, argv[i]);
            } else {
                fprintf(stderr, "%s: %s: %s\n", argv[0], argv[i], error);
                ret = 2;
                continue;
            }
        }
    }

    return ret;
}
