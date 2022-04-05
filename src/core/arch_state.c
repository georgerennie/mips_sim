#include "arch_state.h"
#include <stdio.h>
#include "util/esbmc_util.h"
#include "util/log.h"

void mips_state_init(mips_state_t* state) {
	*state = (mips_state_t){
	    .pc  = 0,
	    .gpr = {0},
	};
}

uint32_t gpr_read(const mips_state_t* state, uint8_t reg) {
	log_assert(reg < 32);
	return reg == 0 ? 0 : state->gpr[reg];
}

void gpr_write(mips_state_t* state, uint8_t reg, uint32_t value) {
	log_assert(reg < 32);
	if (reg > 0) { log_dbg("Writing %d to reg %d\n", value, reg); }
	if (reg > 0) { state->gpr[reg] = value; }
}
