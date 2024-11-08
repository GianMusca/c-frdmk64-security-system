#include "base64.h"

static const char table_base64[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  '+', '/'
};

size_t encode_base64(char* target, size_t target_len, const char* source, size_t source_len)
{
	size_t i;
	int8_t j;
	size_t target_idx = 0;
	size_t longer = (source_len % 3) ? (3 - (source_len % 3)) : 0;
	size_t source_len_b64 = source_len + longer;
	size_t len = (((source_len_b64) * 4) / 3);
	uint8_t x = 5;
	uint8_t current = 0;

	for (i = 0; i < source_len_b64; i++) {
		uint8_t b = (i < source_len ? (uint8_t)source[i] : 0);
		for (j = 7; j >= 0; j--, x--) {
			if ((b & (1 << j)) != 0) {
				current = (uint8_t)(current | (1U << x));
			}
			if (x == 0) {
				target[target_idx++] = table_base64[current];
				x = 6;
				current = 0;
			}
		}
	}
	for (i = len - longer; i < len; i++) {
		target[i] = '=';
	}
	return len;
}
