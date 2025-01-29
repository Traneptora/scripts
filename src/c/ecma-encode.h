#ifndef _ECMA_ENCODE_H
#define _ECMA_ENCODE_H

int ecma_encode_uri_component(char **dest, const char *input);
int ecma_encode_uri(char **dest, const char *input);
int ecma_decode(char **dest, const char *input);

#endif // _ECMA_ENCODE_H
