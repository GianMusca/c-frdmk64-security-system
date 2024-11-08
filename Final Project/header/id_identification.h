#ifndef ID_IDENTIFICATION_H_
#define ID_IDENTIFICATION_H_

#include <stdbool.h>

#define N_CARDS	10
#define ID_SIZE	8

extern char Cards[N_CARDS][ID_SIZE+1];

bool search_id_in_list(const char* id);

#endif
