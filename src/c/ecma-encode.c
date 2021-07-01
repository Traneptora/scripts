#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "utf8.h"
#include "ecma-encode.h"

// (if deadlink) https://web.archive.org/web/20210622004141/
// https://tc39.es/ecma262/multipage/global-object.html#sec-uri-handling-functions
int ecma_encode(char **dest, char *const input, const char *unescaped_set){
    
    int status = 0;

    const size_t str_len = strlen(input);

    char hex[5];
    // largest encoded string is 3 times larger, for % and two hex
    char *ret = (char *) calloc(str_len + 1, 3); // R, in the spec

    if (!ret){
        status |= ECMA_CODE_ALLOC;
        goto encode_fail;
    }

    size_t ret_offset = 0;
    size_t byte_offset = 0; // k, in the spec
    size_t garbo;
    
    while (1){
        if (byte_offset >= str_len){
            ret[ret_offset] = '\0';
            *dest = ret;
            return ECMA_CODE_OK;
        }
        uint32_t c = u8_nextchar(input, &byte_offset);
        if (c && u8_strchr(unescaped_set, c, &garbo)){
            ret_offset += u8_wc_toutf8(ret + ret_offset, c);
        } else {
            size_t len = u8_wc_toutf8(hex, c);
            for (size_t i = 0; i < len; i++){
                snprintf(ret + ret_offset, 4, "%%%02X", 0xFFU & (uint32_t)hex[i]);
                ret_offset += 3;
            }
        }
    }

encode_fail:
    if (ret)
        free(ret);
    return status;

}

int ecma_encode_uri_component(char **dest, char *const input){
    return ecma_encode(dest, input, ecma_uri_component_set);
}

int ecma_encode_uri(char **dest, char *const input){
    return ecma_encode(dest, input, ecma_uri_set);
}

int ecma_decode(char **dest, char *const input){

    int status = 0;

    const size_t str_len = strlen(input);
    char hex[3];
    char *ret = (char *) calloc(str_len + 1, 1); // R, in the spec
    if (!ret){
        status |= ECMA_CODE_ALLOC;
        goto decode_fail;
    }
    hex[2] = '\0';
    size_t ret_offset = 0;
    size_t byte_offset = 0; // k, in the spec
    size_t garbo;
    
    while (1){
        if (byte_offset >= str_len){
            ret[ret_offset] = '\0';
            *dest = ret;
            return ECMA_CODE_OK;
        }
        uint32_t c = u8_nextchar(input, &byte_offset);
        if (c != 0x0025U){ // '%'
            ret_offset += u8_wc_toutf8(ret + ret_offset, c);
        } else {
            if (byte_offset + 1 >= str_len){
                status |= ECMA_CODE_BADFORMAT;
                goto decode_fail;
            }

            hex[0] = input[byte_offset];
            hex[1] = input[byte_offset + 1];
            // This is to prevent scanf from grabbing crap like 0x or -5
            if (!CHAR_IN_SET("0123456789abcdefABCDEF", hex[0]) || !CHAR_IN_SET("0123456789abcdefABCDEF", hex[1])){
                status |= ECMA_CODE_BADFORMAT;
                goto decode_fail;
            }
            unsigned int hex_value;
            int num_found = sscanf(hex, "%x", &hex_value);
            if (num_found != 1){
                status |= ECMA_CODE_BADFORMAT;
                goto decode_fail;
            }
            byte_offset += 2;
            ret[ret_offset++] = (char)(0xFFU & hex_value);
        }
    }

decode_fail:
    if (ret)
        free(ret);
    return status;
}
