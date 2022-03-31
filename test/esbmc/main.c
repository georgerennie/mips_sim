#include "core/core.h"
#include "util/esbmc_util.h"

#define NUM_INSTRS 10

int main() {
	mips_core_t core;

	uint32_t instr_mem[NUM_INSTRS];
	for (uint8_t i = 1; i < NUM_INSTRS; i++) { instr_mem[i] = nondet_u32(); }

	uint8_t data_mem[4];
	mips_core_init(&core, instr_mem, sizeof(instr_mem), ARRAY_TO_SPAN(data_mem));

	for (int i = 0; i < 10; i++) { mips_core_cycle(&core); }
}
