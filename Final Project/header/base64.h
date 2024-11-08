#ifndef BASE64_H
#define BASE64_H

#include <stdio.h>
#include <stdint.h>

size_t encode_base64(char* target, size_t target_len, const char* source, size_t source_len);

#endif
