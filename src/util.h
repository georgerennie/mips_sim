#ifndef _UTIL_H_
#define _UTIL_H_

#include <inttypes.h>
#include <stddef.h>

// Extract a bit range from an integer, like VAL[UPPER:LOWER] in verilog
#define EXTRACT_BITS(UPPER, LOWER, VAL) ((((1ULL << ((UPPER) + 1)) - 1) & VAL) >> LOWER)

// Data accompanied with length field
typedef struct {
	uint8_t* data;
	size_t   size;
} span_t;

#define ARRAY_TO_SPAN(ARRAY) \
	(span_t) { .data = ARRAY, .size = sizeof(ARRAY) }

// Get bounds checked pointer to element in span
uint8_t* span_e(span_t span, size_t idx);

// Infallible malloc, terminates program if allocation fails
void* xmalloc(size_t size);

// Infallible realloc, terminates program if allocation fails
void* xrealloc(void* ptr, size_t new_size);

#endif
