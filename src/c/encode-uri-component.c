#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "ecma-encode.h"

int main(int argc, char **argv){
    char *dest = NULL;
    size_t count = 0;

    if (argc < 2) {
        fprintf(stderr, "%s string...\n", argv[0]);
        return 1;
    }

    while (++count < argc) {
        int status = ecma_encode_uri_component(&dest, argv[count]);
        if (status < 0) {
            fprintf(stderr, "%s: error: %s: \n", argv[0], argv[count]);
            if (dest)
                free(dest);
            return 2;
        }
        printf("%s\n", dest);
        free(dest);
        dest = NULL;
    }

    return 0;
}
