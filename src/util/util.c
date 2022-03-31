#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include "log.h"

inline uint8_t* span_e(span_t span, size_t idx) {
	log_assert(idx < span.size);
	return &span.data[idx];
}

inline void* xmalloc(size_t size) {
	void* ptr = malloc(size);
	if (ptr == NULL) {
		fputs("XMALLOC ERROR: malloc allocation failed\n", stderr);
		exit(EXIT_FAILURE);
	}
	return ptr;
}

inline void* xrealloc(void* ptr, size_t new_size) {
	ptr = realloc(ptr, new_size);
	if (ptr == NULL) {
		fputs("XREALLOC ERROR: realloc allocation failed\n", stderr);
		exit(EXIT_FAILURE);
	}
	return ptr;
}
