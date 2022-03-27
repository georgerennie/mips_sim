#include "util.h"
#include <stdlib.h>
#include <stdio.h>

// Infallible malloc, terminates program if allocation fails
inline void* xmalloc(size_t size) {
	void* p = malloc(size);
	if (p == NULL) {
		fputs("xmalloc: malloc allocation failed\n", stderr);
		exit(EXIT_FAILURE);
	}
	return p;
}
