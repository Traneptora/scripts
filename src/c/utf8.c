#include <stddef.h>
#include <stdint.h>

int utf8_read_codepoint(const char *str, size_t bufsize, uint32_t *codepoint) {
    const uint8_t *uns_str = str;

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

int utf8_write_codepoint(char *str, size_t bufsize, uint32_t codepoint) {
    if (codepoint < 0x80u) {
        if (!bufsize)
            return 0;
        *str = codepoint;
        return 1;
    }

    if (codepoint < 0x800u) {
        if (bufsize < 2)
            return 0;
        str[0] = ((codepoint >> 6) & 0x1fu) | 0xc0u;
        str[1] = (codepoint & 0x3fu) | 0x80u;
        return 2;
    }

    if (codepoint < 0x10000u) {
        if (bufsize < 3)
            return 0;
        str[0] = ((codepoint >> 12) & 0x0fu) | 0xe0u;
        str[1] = ((codepoint >> 6) & 0x3fu) | 0x80u;
        str[2] = (codepoint & 0x3fu) | 0x80u;
        return 3;
    }

    if (codepoint < 0x110000u) {
        if (bufsize < 4)
            return 0;
        str[0] = ((codepoint >> 18) & 0x07u) | 0xf0u;
        str[1] = ((codepoint >> 12) & 0x3fu) | 0x80u;
        str[2] = ((codepoint >> 6) & 0x3fu) | 0x80u;
        str[3] = (codepoint & 0x3fu) | 0x80u;
        return 4;
    }

    return -1;
}
