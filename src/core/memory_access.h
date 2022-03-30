#ifndef _MEMORY_ACCESS_H_
#define _MEMORY_ACCESS_H_

#include <inttypes.h>
#include "util.h"
#include "writeback.h"

typedef enum {
	MEM_ACCESS_NONE,
	MEM_ACCESS_READ_UNSIGNED,
	MEM_ACCESS_READ_SIGNED,
	MEM_ACCESS_WRITE,
} memory_access_type_t;

typedef struct {
	memory_access_type_t access_type;
	uint8_t              bytes;

	writeback_bundle_t wb;
} memory_access_bundle_t;

writeback_bundle_t access_memory(const memory_access_bundle_t* bundle, span_t data_mem);
void memory_access_bundle_init(memory_access_bundle_t* bundle);

#endif
