#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stddef.h>

// Extract a bit range from an integer, like VAL[UPPER:LOWER] in verilog
#define EXTRACT_BITS(UPPER, LOWER, VAL) ((((1ULL << ((UPPER) + 1)) - 1) & VAL) >> LOWER)

// Do the opposite of extract bits, mask a value and shift it to a bit position
#define INSERT_BITS(UPPER, LOWER, VAL) ((((1ULL << (((UPPER) - (LOWER)) + 1)) - 1) & VAL) << LOWER)

// Get number of elements of fixed size array
#define NUM_ELEMS(ARR) (sizeof(ARR) / sizeof(*ARR))

#define ATTR_PACKED __attribute__((__packed__))
#define ATTR_UNUSED __attribute__((unused))
#ifndef ESBMC
	#define ATTR_FALLTHROUGH __attribute__((fallthrough))
#else
	#define ATTR_FALLTHROUGH
#endif

// Data accompanied with length field
typedef struct {
	uint8_t* data;
	size_t   size;
} span_t;

#define MAKE_SPAN(ARRAY) \
	(span_t) { .data = (uint8_t*) ARRAY, .size = sizeof(ARRAY) }

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
