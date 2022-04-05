#include <stdbool.h>
#include "core/core.h"
#include "ref_core/ref_core.h"
#include "util/esbmc_util.h"

#define NUM_INSTRS        2
#define MEM_SIZE          4
#define UNWIND_ITERATIONS 6
#define REF_ITERATIONS    3

int main() {
	mips_core_t     core;
	mips_ref_core_t ref_core;

	uint8_t instr_mem[NUM_INSTRS * sizeof(uint32_t)];
	for (size_t i = 0; i < sizeof(instr_mem) / sizeof(*instr_mem); i++) {
		instr_mem[i] = nondet_u8();
	}

	uint8_t data_mem[MEM_SIZE];
	uint8_t ref_data_mem[MEM_SIZE];
	mips_core_init(&core, ARRAY_TO_SPAN(instr_mem), ARRAY_TO_SPAN(data_mem));
	ref_core_init(&ref_core, ARRAY_TO_SPAN(instr_mem), ARRAY_TO_SPAN(ref_data_mem));

	mips_trap_t trap = false;
	for (uint8_t i = 0; i < UNWIND_ITERATIONS; i++) {
		trap = mips_core_cycle(&core);
		if (trap != MIPS_TRAP_NONE) { break; }
	}

	mips_trap_t ref_trap = false;
	for (uint8_t i = 0; i < REF_ITERATIONS; i++) {
		ref_trap = ref_core_cycle(&ref_core);
		if (ref_trap != MIPS_TRAP_NONE) { break; }
	}

	log_assert(trap != MIPS_TRAP_NONE);
	log_assert(ref_trap != MIPS_TRAP_NONE);
	log_assert(trap == ref_trap);

	log_assert(core.state.gpr[0] == ref_core.state.gpr[0]);
	log_assert(core.state.gpr[1] == ref_core.state.gpr[1]);
	log_assert(core.state.gpr[2] == ref_core.state.gpr[2]);
	log_assert(core.state.gpr[3] == ref_core.state.gpr[3]);
	log_assert(core.state.gpr[4] == ref_core.state.gpr[4]);
	log_assert(core.state.gpr[5] == ref_core.state.gpr[5]);
	log_assert(core.state.gpr[6] == ref_core.state.gpr[6]);
	log_assert(core.state.gpr[7] == ref_core.state.gpr[7]);
	log_assert(core.state.gpr[8] == ref_core.state.gpr[8]);
	log_assert(core.state.gpr[9] == ref_core.state.gpr[9]);
	log_assert(core.state.gpr[10] == ref_core.state.gpr[10]);
	log_assert(core.state.gpr[11] == ref_core.state.gpr[11]);
	log_assert(core.state.gpr[12] == ref_core.state.gpr[12]);
	log_assert(core.state.gpr[13] == ref_core.state.gpr[13]);
	log_assert(core.state.gpr[14] == ref_core.state.gpr[14]);
	log_assert(core.state.gpr[15] == ref_core.state.gpr[15]);
	log_assert(core.state.gpr[16] == ref_core.state.gpr[16]);
	log_assert(core.state.gpr[17] == ref_core.state.gpr[17]);
	log_assert(core.state.gpr[18] == ref_core.state.gpr[18]);
	log_assert(core.state.gpr[19] == ref_core.state.gpr[19]);
	log_assert(core.state.gpr[10] == ref_core.state.gpr[10]);
	log_assert(core.state.gpr[21] == ref_core.state.gpr[21]);
	log_assert(core.state.gpr[22] == ref_core.state.gpr[22]);
	log_assert(core.state.gpr[23] == ref_core.state.gpr[23]);
	log_assert(core.state.gpr[24] == ref_core.state.gpr[24]);
	log_assert(core.state.gpr[25] == ref_core.state.gpr[25]);
	log_assert(core.state.gpr[26] == ref_core.state.gpr[26]);
	log_assert(core.state.gpr[27] == ref_core.state.gpr[27]);
	log_assert(core.state.gpr[28] == ref_core.state.gpr[28]);
	log_assert(core.state.gpr[29] == ref_core.state.gpr[29]);
	log_assert(core.state.gpr[30] == ref_core.state.gpr[30]);
	log_assert(core.state.gpr[31] == ref_core.state.gpr[31]);
}
