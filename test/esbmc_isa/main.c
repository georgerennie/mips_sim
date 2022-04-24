#include <stdint.h>
#include "common/esbmc_util.h"
#include "common/instruction.h"
#include "common/util.h"
#include "core/core.h"
#include "equiv_check/equiv_check.h"
#include "ref_core/ref_core.h"

int main() {
	uint8_t   instr_mem[64] = {0};
	uint32_t* instr_mem_32  = (uint32_t*) instr_mem;

	instr_mem_32[0]  = nondet_u32();
	instr_mem_32[1]  = nondet_u32();
	instr_mem_32[2]  = nondet_u32();
	instr_mem_32[3]  = nondet_u32();
	instr_mem_32[4]  = nondet_u32();
	instr_mem_32[5]  = nondet_u32();
	instr_mem_32[6]  = nondet_u32();
	instr_mem_32[7]  = nondet_u32();
	instr_mem_32[8]  = nondet_u32();
	instr_mem_32[9]  = nondet_u32();
	instr_mem_32[10] = nondet_u32();
	instr_mem_32[11] = nondet_u32();
	instr_mem_32[12] = nondet_u32();
	instr_mem_32[13] = nondet_u32();
	instr_mem_32[14] = nondet_u32();
	instr_mem_32[15] = nondet_u32();

	const bool delay_slots = nondet_bool();

	uint8_t             data_mem[16];
	const mips_config_t config = {
	    .delay_slots = delay_slots,
	    .instr_mem   = MAKE_SPAN(instr_mem),
	    .data_mem    = MAKE_SPAN(data_mem),
	};

	mips_core_t core;
	mips_core_init(&core, config);

	for (uint8_t i = 0; i < 8; i++) {
		mips_retire_metadata_t retire = mips_core_cycle(&core);
		esbmc_assume(!retire.exception.raised);
	}

	// Run one cycle, observing its behaviour
	const mips_state_t           before_state = core.state;
	const mips_retire_metadata_t retire       = mips_core_cycle(&core);
	esbmc_assume(retire.active);

	// We aren't tracking memory so cant do loads currently
	mips_opcode_t opc = EXTRACT_BITS(31, 26, retire.instruction);
	esbmc_assume(opc != MIPS_OPC_LHU);

	mips_config_t   ref_config = config;
	mips_ref_core_t ref_core;
	// Setup reference core to execute just executed instruction
	ref_core_init(&ref_core, ref_config);
	ref_core.state    = before_state;
	ref_core.state.pc = retire.address;

	// Run one cycle of reference
	const mips_retire_metadata_t ref_retire = ref_core_cycle(&ref_core);

	// Check registers are the same
	equiv_check_retires_pic(&retire, &ref_retire);
	equiv_check_gprs(&core, &ref_core);
}
