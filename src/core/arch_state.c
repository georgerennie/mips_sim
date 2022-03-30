#include "arch_state.h"
#include <assert.h>
#include <stdio.h>
#include "esbmc_util.h"

void mips_state_init(mips_state_t* state) {
	*state = (mips_state_t){
	    .pc  = 0,
	    .gpr = {0},
	};

#ifdef __ESBMC
	// Allow any initial register state for ESBMC
	for (uint8_t i = 0; i < sizeof(state->gpr) / sizeof(state->gpr[0]); i++) {
		state->gpr[i] = nondet_u32();
	}
#endif
}

uint32_t gpr_read(const mips_state_t* state, uint8_t reg) {
	assert(reg < 32);
	return reg == 0 ? 0 : state->gpr[reg - 1];
}

void gpr_write(mips_state_t* state, uint8_t reg, uint32_t value) {
	assert(reg < 32);
	if (reg > 0) { state->gpr[reg - 1] = value; }
}

void print_gprs(const mips_state_t* state) {
#ifndef __ESBMC
	puts("GPR state:");
	for (uint8_t i = 0; i < 32; i++) { printf("%d: 0x%08X\n", i, gpr_read(state, i)); }
#endif
}
