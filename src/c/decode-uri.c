#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int utf8_read_codepoint(const char *str, size_t bufsize, uint32_t *codepoint) {
    const uint8_t *uns_str = (const uint8_t *) str;

    if (!bufsize)
        return 0;

    if (!(*uns_str & 0x80u)) {
        *codepoint = *uns_str;
        return 1;
    }

    if ((*uns_str & 0xe0u) == 0xc0u) {
        if (bufsize < 2)
            return 0;
        if ((uns_str[1] & 0xc0u) != 0x80u)
            return -1;
        *codepoint = ((0x1fu & (uint32_t)uns_str[0]) << 6) | (0x3fu & (uint32_t)uns_str[1]);
        return 2;
    }

    if ((*uns_str & 0xf0u) == 0xe0u) {
        if (bufsize < 3)
            return 0;
        if (((uns_str[1] & 0xc0u) != 0x80u) || ((uns_str[2] & 0xc0u) != 0x80u))
            return -1;
        *codepoint = ((0x0fu & (uint32_t)uns_str[0]) << 12)
            | ((0x3fu & (uint32_t)uns_str[1]) << 6)
            | (0x3fu & (uint32_t)uns_str[2]);
        return 3;
    }

    if ((*uns_str & 0xf8u) == 0xf0u) {
        if (bufsize < 4)
            return 0;
        if (((uns_str[1] & 0xc0u) != 0x80u) || ((uns_str[2] & 0xc0u) != 0x80u) || ((uns_str[3] & 0xc0u) != 0x80u))
            return -1;
        *codepoint = ((0x07u & (uint32_t)uns_str[0]) << 18)
            | ((0x3fu & (uint32_t)uns_str[1]) << 12)
            | ((0x3fu & (uint32_t)uns_str[2]) << 6)
            | (0x3fu & (uint32_t)uns_str[3]);
        return 3;
    }

    // invalid utf-8
    return -1;
}

static inline int hex_to_int(uint8_t hex) {
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 0xau;
    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 0xau;
    return -1;
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

int main(int argc, char **argv){
    char *dest = NULL;
    size_t count = 0;

    if (argc < 2) {
        fprintf(stderr, "%s string...\n", argv[0]);
        return 1;
    }

    while (++count < argc) {
        int status = ecma_decode(&dest, argv[count]);
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
