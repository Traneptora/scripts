#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "utf8.h"
#include "ecma-encode.h"

int main(int argc, char **argv){
    char *dest;
    int status = 0;
    if (argc > 1){
        size_t count = 0;
        while (argc > ++count){
            status |= ecma_encode_uri_component(&dest, argv[count]);
            if (status != 0){
                fprintf(stderr, "%s: error: %d: %s: \n", "encode-uri-component", status, argv[count]);
                return status;
            }
            printf("%s\n", dest);
        }
    } else {
        fprintf(stderr, "%s string...\n", "encode-uri-component");
        return 1;
    }
    return status;
}
