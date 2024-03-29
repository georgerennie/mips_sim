#ifndef REF_CORE_H
#define REF_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "common/arch.h"
#include "common/util.h"

typedef struct {
	mips_state_t state;

	uint32_t cycle;

	mips_config_t config;

	// When emulating delay slots, this is set by a jump/branch instruction,
	// causing the core to jump to branch_dest in the cycle after branch_after
	// is set
	bool     branch_after;
	uint32_t branch_dest;
} mips_ref_core_t;

void ref_core_init(mips_ref_core_t* core, mips_config_t config);

// Execute one instruction
mips_retire_metadata_t ref_core_cycle(mips_ref_core_t* state);

#ifdef __cplusplus
}
#endif

#endif
