/**
 * @file slurp.c
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

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SB_INIT_BUFSIZE ((size_t)4096)
#define SB_MAX_CAPACITY ((size_t)1u << 31)

typedef struct {
    uint8_t *buf;
    size_t buflen;
    size_t capacity;
} SlurpBuffer;

static void free_slurpbuffer(SlurpBuffer **sb) {
    if (!sb)
        return;
    if (!*sb)
        return;
    if ((*sb)->buf)
        free((*sb)->buf);
    (*sb)->buf = NULL;
    free(*sb);
    *sb = NULL;
}

static SlurpBuffer *new_slurpbuffer(size_t capacity) {
    SlurpBuffer *sb = NULL;

    if (capacity > SB_MAX_CAPACITY)
        return NULL;

    sb = malloc(sizeof(SlurpBuffer));
    if (!sb)
        goto fail;
    
    sb->buf = malloc(capacity);
    if (!sb->buf)
        goto fail;

    sb->capacity = capacity;
    sb->buflen = 0;
    return sb;

fail:
    free_slurpbuffer(&sb);
    return NULL;
}

static SlurpBuffer *get_larger_slurpbuffer(SlurpBuffer *sb) {
    SlurpBuffer *new_sb = NULL;
    size_t new_capacity = sb->capacity * 2;  
    if (new_capacity <= sb->capacity)
        /* overrun */
        return NULL;
    new_sb = new_slurpbuffer(new_capacity);
    if (!new_sb)
        return NULL;
    memcpy(new_sb->buf, sb->buf, sb->buflen);
    new_sb->buflen = sb->buflen;
    free_slurpbuffer(&sb);
    return new_sb;
}

int main(int argc, char **argv) {
    size_t bytes_read, bytes_written;
    size_t remaining;
    FILE *fout = stdout;
    SlurpBuffer *sb = new_slurpbuffer(SB_INIT_BUFSIZE);
    int ret;

    while (1) {
        remaining = sb->capacity - sb->buflen;
        if (!remaining) {
            sb = get_larger_slurpbuffer(sb);
            if (!sb) {
                fprintf(stderr, "%s: could not allocate buffer\n", argv[0]);
                ret = 3;
                goto fail;
            }
            remaining = sb->capacity - sb->buflen;
        }
        bytes_read = fread(sb->buf + sb->buflen, 1, remaining, stdin);
        if (bytes_read < remaining) {
            if (ferror(stdin)) {
                perror(argv[0]);
                ret = 1;
                goto fail;
            }
        }
        sb->buflen += bytes_read;
        if (feof(stdin)) {
            fclose(stdin);
            break;
        }
    }

    remaining = sb->buflen;

    if (argc > 1) {
        fout = fopen(argv[1], "wb");
        if (!fout) {
            perror(argv[0]);
            ret = 1;
            goto fail;
        }
    }

    while (remaining > 0) {
        bytes_written = fwrite(sb->buf + sb->buflen - remaining, 1, remaining, fout);
        if (bytes_written < remaining) {
            if (ferror(fout)) {
                perror(argv[0]);
                ret = 2;
                goto fail;
            }
        }
        remaining -= bytes_written;
    }

    fclose(fout);
    free_slurpbuffer(&sb);

    return 0;

fail:
    free_slurpbuffer(&sb);
    if (fout && fout != stdout)
        fclose(fout);
    return ret;
}
