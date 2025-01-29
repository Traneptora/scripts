#ifndef _UTF8_H
#define _UTF8_H

#include <stddef.h>
#include <stdint.h>

int utf8_read_codepoint(const char *str, size_t bufsize, uint32_t *codepoint);
int utf8_write_codepoint(char *str, size_t bufsize, uint32_t codepoint);

#endif _UTF8_H
