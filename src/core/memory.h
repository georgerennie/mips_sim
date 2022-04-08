#ifndef MEMORY_H
#define MEMORY_H

#include <inttypes.h>
#include "util/util.h"
#include "writeback.h"

typedef enum {
	MEM_ACCESS_NONE = 0,
	MEM_ACCESS_READ_UNSIGNED,
	MEM_ACCESS_READ_SIGNED, // Currently unused
	MEM_ACCESS_WRITE,
} memory_access_t;

typedef struct {
	// Control
	memory_access_t access_type;
	uint8_t         bytes; // Number of bytes to R/W. No R/W if 0

	// Data
	uint32_t data_rt; // RT value for writeback

	// Data / Pipeline passthrough
	// Destination reg is set to 0 if memory store occurs
	mem_wb_reg_t mem_wb;
} ex_mem_reg_t;

mem_wb_reg_t memory(const ex_mem_reg_t* ex_mem, span_t data_mem);
void         ex_mem_reg_init(ex_mem_reg_t* ex_mem);

#endif
