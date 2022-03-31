#ifndef _CORE_H_
#define _CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "arch_state.h"
#include "execution_engine.h"
#include "instruction_decoder.h"
#include "memory_access.h"
#include "util/util.h"
#include "writeback.h"

typedef struct {
	mips_state_t state;

	uint32_t* instr_mem;
	size_t    instr_mem_size; // In bytes
	span_t    data_mem;

	uint32_t               decoded_instruction;
	execute_bundle_t       exec_bundle;
	memory_access_bundle_t mem_bundle;
	writeback_bundle_t     wb_bundle;
} mips_core_t;

void mips_core_init(mips_core_t* core, uint32_t* instr_mem, size_t instr_mem_size, span_t data_mem);

// Execute one clock cycle
void mips_core_cycle(mips_core_t* core);

#ifdef __cplusplus
}
#endif

#endif
