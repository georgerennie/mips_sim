#ifndef _CORE_H_
#define _CORE_H_

#include "arch_state.h"
#include "execution_engine.h"
#include "instruction_decoder.h"
#include "memory_access.h"
#include "util.h"

typedef struct {
	mips_state_t state;

	uint32_t* instr_mem;
	size_t    instr_mem_size; // In bytes
	span_t    data_mem;

	decode_bundle_t        decode_out;
	execution_bundle_t     execute_out;
	memory_access_bundle_t memory_out;
} mips_core_t;

void mips_core_init(mips_core_t* core, uint32_t* instr_mem, size_t instr_mem_size, span_t data_mem);

// Execute one clock cycle
void mips_core_cycle(mips_core_t* core);

#endif
