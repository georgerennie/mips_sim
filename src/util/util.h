#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
extern "C++" {
	#include <span>

inline span_t make_c_span(std::span<uint8_t> s) { return {s.data(), s.size_bytes()}; }
}
#endif

// Get bounds checked pointer to element in span. Indexing is done in bytes
// so index needs to be multiplied by sizeof(T)
uint8_t* span_e(span_t span, size_t idx);

// Infallible malloc, terminates program if allocation fails
void* xmalloc(size_t size);

// Infallible realloc, terminates program if allocation fails
void* xrealloc(void* ptr, size_t new_size);

#ifdef __cplusplus
}
#endif

#endif
