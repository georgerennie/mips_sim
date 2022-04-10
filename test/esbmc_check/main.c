// General check of useful single threaded properties, as well as embedded
// properties with esbmc. Runs on the whole program

#include <stdbool.h>
#include "core/core.h"
#include "util/esbmc_util.h"

#define NUM_INSTRS        10
#define MEM_SIZE          16
#define UNWIND_ITERATIONS (NUM_INSTRS + 5)

int main() {
	mips_core_t core;

	uint8_t instr_mem[NUM_INSTRS * sizeof(uint32_t)];
	for (size_t i = 0; i < NUM_ELEMS(instr_mem); i++) { instr_mem[i] = nondet_u8(); }

	uint8_t       data_mem[MEM_SIZE];
	mips_config_t config = {
	    .delay_slots = nondet_bool(),
	    .instr_mem   = MAKE_SPAN(instr_mem),
	    .data_mem    = MAKE_SPAN(data_mem),
	};
	mips_core_init(&core, config);

	// mips_trap_t trap = false;
	for (uint8_t i = 0; i < UNWIND_ITERATIONS; i++) {
		// trap = mips_core_cycle(&core).trap;
		// if (trap != MIPS_TRAP_NONE) { break; }
		mips_core_cycle(&core);
	}

	// log_assert_neqi(trap, MIPS_TRAP_NONE);
}
