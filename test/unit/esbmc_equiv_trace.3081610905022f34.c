#include <stdbool.h>
#include "core/core.h"
#include "ref_core/ref_core.h"
#include "util/esbmc_util.h"

#define MEM_SIZE   4
#define CYCLES     7
#define REF_CYCLES 3

#ifdef UNIT_TEST
	#include "unit/test.h"
DEFINE_TEST(trace3081610905022f34) {
#else
int main() {
#endif
	mips_core_t     core;
	mips_ref_core_t ref_core;

	uint8_t instr_mem[8];

	instr_mem[0] = 48;
	instr_mem[1] = 129;
	instr_mem[2] = 97;
	instr_mem[3] = 9;
	instr_mem[4] = 5;
	instr_mem[5] = 2;
	instr_mem[6] = 47;
	instr_mem[7] = 52;

	uint8_t data_mem[MEM_SIZE];
	uint8_t ref_data_mem[MEM_SIZE];

	mips_core_init(&core, ARRAY_TO_SPAN(instr_mem), ARRAY_TO_SPAN(data_mem));
	ref_core_init(&ref_core, ARRAY_TO_SPAN(instr_mem), ARRAY_TO_SPAN(ref_data_mem));

	mips_trap_t trap = false;
	for (size_t i = 0; i < CYCLES; i++) {
		trap = mips_core_cycle(&core);
		if (trap != MIPS_TRAP_NONE) { break; }
	}

	mips_trap_t ref_trap = false;
	for (size_t i = 0; i < REF_CYCLES; i++) {
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
	log_assert(core.state.gpr[20] == ref_core.state.gpr[20]);
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
