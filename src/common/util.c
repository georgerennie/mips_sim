#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include "log.h"

inline uint8_t* span_e(span_t span, size_t idx) {
	log_assert_lt(idx, span.size, "%zu");
	return &span.data[idx];
}

inline void* xmalloc(size_t size) {
	void* ptr = malloc(size);
	log_assert_fmt(ptr != NULL, "xmalloc allocation failed");
	return ptr;
}

inline void* xrealloc(void* ptr, size_t new_size) {
	ptr = realloc(ptr, new_size);
	log_assert_fmt(ptr != NULL, "xrealloc allocation failed");
	return ptr;
}
