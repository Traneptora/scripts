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

static inline void freep(void *p) {
    void **vp = p;
    if (!vp || !*vp)
        return;
    free(*vp);
    *vp = NULL;
};

static void free_slurpbuffer(SlurpBuffer **sb) {
    if (!sb)
        return;
    free((*sb)->buf);
    freep(sb);
}

static SlurpBuffer *new_slurpbuffer(size_t capacity) {
    SlurpBuffer *sb = NULL;

    if (capacity > SB_MAX_CAPACITY)
        return NULL;

    sb = malloc(sizeof(*sb));
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

static int slurp_from(const char *argv0, FILE *in, SlurpBuffer **sbp)
{
    size_t bytes_read;
    SlurpBuffer *sb = *sbp;
    int ret = 0;

    while (1) {
        size_t remaining = sb->capacity - sb->buflen;
        if (!remaining) {
            sb = get_larger_slurpbuffer(sb);
            if (!sb) {
                fprintf(stderr, "%s: could not allocate buffer\n", argv0);
                ret = ENOMEM;
                goto end;
            }
            *sbp = sb;
            remaining = sb->capacity - sb->buflen;
        }
        bytes_read = fread(sb->buf + sb->buflen, 1, remaining, in);
        if (bytes_read < remaining && ferror(in)) {
            perror(argv0);
            ret = errno;
            goto end;
        }
        sb->buflen += bytes_read;
        if (feof(in))
            break;
    }

end:
    fclose(in);
    return ret;
}

int main(int argc, const char *const *argv) {
    const char *fout_name = NULL;
    SlurpBuffer *sb;
    FILE *fout = NULL;
    const char *const *fnamep = NULL;
    int fnamec = 0;
    int ret = 0, mmoff = 1;

    if (argc > 1 && !strncmp(argv[1], "--o=", 4)) {
        fout_name = argv[1] + 4;
        mmoff = 2;
    }

    if (argc > mmoff) {
        if (!strcmp(argv[mmoff], "--")) {
            if (argc > mmoff + 1) {
                fnamep = argv + mmoff + 1;
                fnamec = argc - (mmoff + 1);
            }
        } else {
            fnamep = argv + mmoff;
            fnamec = argc - mmoff;
        }
    }

    sb = new_slurpbuffer(SB_INIT_BUFSIZE);
    if (!sb) {
        ret = ENOMEM;
        goto end;
    }

    do {
        FILE *fin = stdin;
        if (fnamep && fnamec > 0) {
            fin = fopen(*fnamep, "rb");
            if (!fin)
                goto end;
        }
        ret = slurp_from(argv[0], fin, &sb);
        if (ret)
            goto end;
    } while (fnamep++ && --fnamec > 0);

    if (fout_name) {
        fout = fopen(fout_name, "wb");
        if (!fout)
            goto end;
    } else {
        fout = stdout;
    }

    size_t written = fwrite(sb->buf, 1, sb->buflen, fout);
    if (written < sb->buflen && ferror(fout)) {
        perror(argv[0]);
        ret = errno;
    }

end:
    free_slurpbuffer(&sb);
    if (fout)
        fclose(fout);
    return ret;
}
