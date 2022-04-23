#include <stdbool.h>
#include "common/esbmc_util.h"
#include "core/core.h"
#include "equiv_check/equiv_check.h"
#include "ref_core/ref_core.h"

#define MEM_SIZE   4
#define CYCLES     7
#define REF_CYCLES 3

#ifdef UNIT_TEST
	#include "unit/test.h"
DEFINE_TEST(trace008496020084a6) {
#else
int main() {
#endif
	mips_core_t     core;
	mips_ref_core_t ref_core;

	uint8_t instr_mem[8] = {0};

	instr_mem[0] = 2;
	instr_mem[1] = 0;
	instr_mem[2] = 132;
	instr_mem[3] = 150;
	instr_mem[4] = 2;
	instr_mem[5] = 0;
	instr_mem[6] = 132;
	instr_mem[7] = 166;

	uint8_t data_mem[MEM_SIZE]     = {0};
	uint8_t ref_data_mem[MEM_SIZE] = {0};

	const bool delay_slots = false;

	mips_config_t config = {
	    .delay_slots = delay_slots,
	    .instr_mem   = MAKE_SPAN(instr_mem),
	    .data_mem    = MAKE_SPAN(data_mem),
	};

	mips_config_t ref_config = {
	    .delay_slots = delay_slots,
	    .instr_mem   = MAKE_SPAN(instr_mem),
	    .data_mem    = MAKE_SPAN(ref_data_mem),
	};

	mips_core_init(&core, config);
	ref_core_init(&ref_core, ref_config);

	// Check testbench has been initialised correctly
	equiv_check_cores(&core, &ref_core);
	equiv_check_instr_mems(&core, &ref_core);

	mips_retire_metadata_t retire = {0};
	for (size_t i = 0; i < CYCLES; i++) {
		const memory_access_t        access_type = core.regs.ex_mem.access_type;
		const mips_retire_metadata_t new_retire  = mips_core_cycle(&core);

		if (new_retire.active) { retire = new_retire; }
		if (retire.exception.raised) {
			// Assume that the next instruction isnt a mem write as that would
			// cause the memory to diverge
			esbmc_assume(access_type != MEM_ACCESS_WRITE);
			break;
		}
	}

	mips_retire_metadata_t ref_retire = {0};
	for (size_t i = 0; i < REF_CYCLES; i++) {
		ref_retire = ref_core_cycle(&ref_core);
		if (ref_retire.active && ref_retire.instruction_number == retire.instruction_number) {
			break;
		}
	}

	equiv_check_retires(&retire, &ref_retire);
	equiv_check_cores(&core, &ref_core);
}
