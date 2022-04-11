#include <stdbool.h>
#include "common/esbmc_util.h"
#include "core/core.h"
#include "ref_core/ref_core.h"

#define MEM_SIZE   4
#define CYCLES     7
#define REF_CYCLES 3

#ifdef UNIT_TEST
	#include "unit/test.h"
DEFINE_TEST(trace21b840240200e226) {
#else
int main() {
#endif
	mips_core_t     core;
	mips_ref_core_t ref_core;

	uint8_t instr_mem[8];

	instr_mem[0] = 33;
	instr_mem[1] = 184;
	instr_mem[2] = 64;
	instr_mem[3] = 36;
	instr_mem[4] = 2;
	instr_mem[5] = 0;
	instr_mem[6] = 226;
	instr_mem[7] = 38;

	uint8_t data_mem[MEM_SIZE];
	uint8_t ref_data_mem[MEM_SIZE];

	mips_core_init(&core, MAKE_SPAN(instr_mem), MAKE_SPAN(data_mem));
	ref_core_init(&ref_core, MAKE_SPAN(instr_mem), MAKE_SPAN(ref_data_mem));

	mips_trap_t trap = false;
	for (size_t i = 0; i < CYCLES; i++) {
		trap = mips_core_cycle(&core).trap;
		if (trap != MIPS_TRAP_NONE) { break; }
	}

	mips_trap_t ref_trap = false;
	for (size_t i = 0; i < REF_CYCLES; i++) {
		ref_trap = ref_core_cycle(&ref_core);
		if (ref_trap != MIPS_TRAP_NONE) { break; }
	}

	log_assert_neqi(trap, MIPS_TRAP_NONE);
	log_assert_neqi(ref_trap, MIPS_TRAP_NONE);
	log_assert_eqi(trap, ref_trap);

	log_assert_eqi(core.state.gpr[0], ref_core.state.gpr[0]);
	log_assert_eqi(core.state.gpr[0], ref_core.state.gpr[0]);
	log_assert_eqi(core.state.gpr[1], ref_core.state.gpr[1]);
	log_assert_eqi(core.state.gpr[2], ref_core.state.gpr[2]);
	log_assert_eqi(core.state.gpr[3], ref_core.state.gpr[3]);
	log_assert_eqi(core.state.gpr[4], ref_core.state.gpr[4]);
	log_assert_eqi(core.state.gpr[5], ref_core.state.gpr[5]);
	log_assert_eqi(core.state.gpr[6], ref_core.state.gpr[6]);
	log_assert_eqi(core.state.gpr[7], ref_core.state.gpr[7]);
	log_assert_eqi(core.state.gpr[8], ref_core.state.gpr[8]);
	log_assert_eqi(core.state.gpr[9], ref_core.state.gpr[9]);
	log_assert_eqi(core.state.gpr[10], ref_core.state.gpr[10]);
	log_assert_eqi(core.state.gpr[11], ref_core.state.gpr[11]);
	log_assert_eqi(core.state.gpr[12], ref_core.state.gpr[12]);
	log_assert_eqi(core.state.gpr[13], ref_core.state.gpr[13]);
	log_assert_eqi(core.state.gpr[14], ref_core.state.gpr[14]);
	log_assert_eqi(core.state.gpr[15], ref_core.state.gpr[15]);
	log_assert_eqi(core.state.gpr[16], ref_core.state.gpr[16]);
	log_assert_eqi(core.state.gpr[17], ref_core.state.gpr[17]);
	log_assert_eqi(core.state.gpr[18], ref_core.state.gpr[18]);
	log_assert_eqi(core.state.gpr[19], ref_core.state.gpr[19]);
	log_assert_eqi(core.state.gpr[20], ref_core.state.gpr[20]);
	log_assert_eqi(core.state.gpr[21], ref_core.state.gpr[21]);
	log_assert_eqi(core.state.gpr[22], ref_core.state.gpr[22]);
	log_assert_eqi(core.state.gpr[23], ref_core.state.gpr[23]);
	log_assert_eqi(core.state.gpr[24], ref_core.state.gpr[24]);
	log_assert_eqi(core.state.gpr[25], ref_core.state.gpr[25]);
	log_assert_eqi(core.state.gpr[26], ref_core.state.gpr[26]);
	log_assert_eqi(core.state.gpr[27], ref_core.state.gpr[27]);
	log_assert_eqi(core.state.gpr[28], ref_core.state.gpr[28]);
	log_assert_eqi(core.state.gpr[29], ref_core.state.gpr[29]);
	log_assert_eqi(core.state.gpr[30], ref_core.state.gpr[30]);
	log_assert_eqi(core.state.gpr[31], ref_core.state.gpr[31]);
}
