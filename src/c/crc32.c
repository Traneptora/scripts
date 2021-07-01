/*******************************************************************************
* crc32.c by elektron
* modernized by thebombzen
*
* Notes:
* This is a hack from someone elses code :
* http://kremlor.net/projects/crc32
* Find the most current version of this application at:
* https://github.com/thebombzen/scripts/
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "tbz-crc32.h"

void usage(char *progname) {
    fprintf(stderr, "%s by elektron and thebombzen\n", progname);
    fprintf(stderr, "    %s [--help] file1 [fileN...]\n\n", progname);
}

void print_crc32(uint32_t crc, char *fname){
    if (fname){
        printf("%08X %s\n", crc, fname);
    } else {
        printf("%08X\n", crc);
    }
}

int main(int argc, char *argv[]) {

    uint32_t crc;
    int success;
    int ret = 0;
    char *error;

    if (argc <= 1 || strcmp("--help", argv[1]) == 0){
        usage(argv[0]);
        return 1;
    }

    if (argc == 2){
        success = tbz_compute_file_crc32(argv[1], &crc, &error);
        if (success == 0){
            print_crc32(crc, NULL);
        } else {
            fprintf(stderr, "%s: %s: %s\n", "crc32", argv[1], error);
            ret = 2;
        }
    } else {
        for (int i = 1; i < argc; i++){
            success = tbz_compute_file_crc32(argv[i], &crc, &error);
            if (success == 0){
                print_crc32(crc, argv[i]);
            } else {
                fprintf(stderr, "%s: %s: %s\n", "crc32", argv[i], error);
                ret = 2;
                continue;
            }
        }
    }
    return ret;
}
