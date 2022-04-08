#ifndef REF_CORE_H
#define REF_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "core/arch_state.h"
#include "util/util.h"

typedef struct {
	mips_state_t state;

	bool delay_slots;

	// When emulating delay slots, this is set by a jump/branch instruction,
	// causing the core to jump to branch_dest in the cycle after branch_after
	// is set
	bool     branch_after;
	uint32_t branch_dest;

	span_t instr_mem;
	span_t data_mem;
} mips_ref_core_t;

void ref_core_init(mips_ref_core_t* core, span_t instr_mem, span_t data_mem, bool delay_slots);

// Execute instructions until one generates a trap
mips_trap_t ref_core_run(mips_ref_core_t* core);

// Execute one instruction
mips_trap_t ref_core_cycle(mips_ref_core_t* state);

#ifdef __cplusplus
}
#endif

#endif
