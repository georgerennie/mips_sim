#ifndef WRITEBACK_H
#define WRITEBACK_H

#include <inttypes.h>
#include "arch_state.h"

typedef struct {
	// Data
	uint8_t  reg; // Register to write to. No write if reg == 0
	uint32_t result;
} mem_wb_reg_t;

void writeback(const mem_wb_reg_t* mem_wb, mips_state_t* arch_state);
void mem_wb_reg_init(mem_wb_reg_t* mem_wb);

#endif
