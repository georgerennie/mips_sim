#ifndef _MEMORY_ACCESS_H_
#define _MEMORY_ACCESS_H_

#include "execution_engine.h"
#include "util.h"

typedef struct {
	uint8_t  writeback_reg;
	uint32_t writeback_value;
} memory_access_bundle_t;

memory_access_bundle_t access_memory(const execution_bundle_t* exec, span_t data_mem);

#endif
