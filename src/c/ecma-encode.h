#include <string.h>

#ifndef _ECMA_ENCODE_H
#define _ECMA_ENCODE_H

// (if deadlink) https://web.archive.org/web/20210622004141/
// https://tc39.es/ecma262/multipage/global-object.html#sec-uri-handling-functions
static const char *ecma_uri_component_set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.!~*'()";
static const char *ecma_uri_set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.!~*'();/?:@&=+$,#";

#define ECMA_CODE_OK 0x00
#define ECMA_CODE_ALLOC 0x02
#define ECMA_CODE_BADFORMAT 0x04

#define CHAR_IN_SET(set, c) ((c) && strchr((set), (c)))

int ecma_encode(char **dest, char *const input, const char *unescaped_set);
int ecma_encode_uri_component(char **dest, char *const input);
int ecma_encode_uri(char **dest, char *const input);

int ecma_decode(char **dest, char *const input);

#endif // _ECMA_ENCODE_H
