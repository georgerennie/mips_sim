#ifndef _REF_CORE_H_
#define _REF_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "core/arch_state.h"
#include "util/util.h"

typedef struct {
	mips_state_t state;

	span_t instr_mem;
	span_t data_mem;
} mips_ref_core_t;

void ref_core_init(mips_ref_core_t* core, span_t instr_mem, span_t data_mem);

// Execute instructions until one generates a trap
mips_trap_t ref_core_run(mips_ref_core_t* core);

// Execute one instruction
mips_trap_t ref_core_cycle(mips_ref_core_t* state);

#ifdef __cplusplus
}
#endif

#endif
