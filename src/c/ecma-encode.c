#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utf8.h"
#include "ecma-encode.h"

static const uint8_t ecma_uri_component_lut[] = {
    0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
};

static const uint8_t ecma_uri_lut[] = {
    0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
};


static inline int hex_to_int(uint8_t hex) {
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 0xau;
    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 0xau;
    return -1;
}

static inline int int_to_hex(uint8_t input) {
    if (input < 10)
        return input + '0';
    if (input < 16)
        return input + 'a';
    return -1;
}

// (if deadlink) https://web.archive.org/web/20210622004141/
// https://tc39.es/ecma262/multipage/global-object.html#sec-uri-handling-functions
static int ecma_encode(char **dest, const char *input, const uint8_t *lut) {
    
    int status = 0;
    size_t str_len = strlen(input);
    char *ret = calloc(str_len + 1, 3);

    if (!ret)
        goto encode_fail;

    size_t ret_offset = 0;
    size_t byte_offset = 0;
    size_t garbo;

    while (byte_offset < str_len) {
        uint32_t codepoint;
        status = utf8_read_codepoint(input + byte_offset, str_len - byte_offset + 1, &codepoint);
        if (status <= 0)
            goto encode_fail;
        if (codepoint >= 32 && codepoint < 127 && lut[codepoint - 32]) {
            ret[ret_offset] = codepoint;
            ret_offset += 1;
        } else {
            for (int i = 0; i < status; i++) {
                uint32_t byte = input[byte_offset + i];
                ret[ret_offset] = '%';
                ret[ret_offset + 1] = int_to_hex((byte >> 4) & 0xfu);
                ret[ret_offset + 2] = int_to_hex(byte & 0xfu);
                ret_offset += 3;
            }
        }
        byte_offset += status;
    }

    ret[ret_offset] = '\0';
    *dest = ret;
    return 0;

encode_fail:
    if (ret)
        free(ret);
    return -1;
}

int ecma_encode_uri_component(char **dest, const char *input) {
    return ecma_encode(dest, input, ecma_uri_component_lut);
}

int ecma_encode_uri(char **dest, const char *input) {
    return ecma_encode(dest, input, ecma_uri_lut);
}

int ecma_decode(char **dest, const char *input){

    int status = 0;
    const size_t str_len = strlen(input);
    char *ret = calloc(str_len + 1, 1); // R, in the spec

    if (!ret)
        goto decode_fail;

    size_t ret_offset = 0;
    size_t byte_offset = 0; // k, in the spec
        
    while (byte_offset < str_len) {
        uint32_t codepoint;
        status = utf8_read_codepoint(input + byte_offset, str_len - byte_offset + 1, &codepoint);
        if (status <= 0)
            goto decode_fail;
        if (codepoint == '%') {
            if (byte_offset + 2 > str_len)
                goto decode_fail;
            int v0 = hex_to_int(input[byte_offset + 1]);
            int v1 = hex_to_int(input[byte_offset + 2]);
            if (v0 < 0 || v1 < 0)
                goto decode_fail;
            ret[ret_offset++] = (v0 << 4) | v1;
            byte_offset += 3;
        } else {
            for (int i = 0; i < status; i++)
                ret[ret_offset + i] = input[byte_offset + i];
            ret_offset += status;
            byte_offset += status;
        }
    }

    ret[ret_offset++] = '\0';
    *dest = ret;
    return 0;

decode_fail:
    if (ret)
        free(ret);
    return -1;
}
