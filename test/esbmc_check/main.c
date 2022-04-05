// General check of useful single threaded properties, as well as embedded
// properties with esbmc. Runs on the whole program

#include <stdbool.h>
#include "core/core.h"
#include "util/esbmc_util.h"

#define NUM_INSTRS        3
#define MEM_SIZE          4
#define UNWIND_ITERATIONS 10

int main() {
	mips_core_t core;

	uint8_t instr_mem[NUM_INSTRS * sizeof(uint32_t)];
	for (size_t i = 0; i < sizeof(instr_mem) / sizeof(*instr_mem); i++) {
		instr_mem[i] = nondet_u8();
	}

	uint8_t data_mem[MEM_SIZE];
	mips_core_init(&core, ARRAY_TO_SPAN(instr_mem), ARRAY_TO_SPAN(data_mem));

	mips_trap_t trap = false;
	for (uint8_t i = 0; i < UNWIND_ITERATIONS; i++) {
		trap = mips_core_cycle(&core);
		if (trap != MIPS_TRAP_NONE) { break; }
	}

	log_assert(trap != MIPS_TRAP_NONE);
}
