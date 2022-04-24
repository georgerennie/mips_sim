#include "equiv_check.h"
#include "common/log.h"

void equiv_check_gprs(const mips_core_t* sim_core, const mips_ref_core_t* ref_core) {
	const mips_state_t* sim_state = &sim_core->state;
	const mips_state_t* ref_state = &ref_core->state;

	log_assert_eqi(sim_state->gpr[0], 0);
	log_assert_eqi(ref_state->gpr[0], 0);

	// Unrolled for ESBMC
	log_assert_eqi(sim_state->gpr[1], ref_state->gpr[1]);
	log_assert_eqi(sim_state->gpr[2], ref_state->gpr[2]);
	log_assert_eqi(sim_state->gpr[3], ref_state->gpr[3]);
	log_assert_eqi(sim_state->gpr[4], ref_state->gpr[4]);
	log_assert_eqi(sim_state->gpr[5], ref_state->gpr[5]);
	log_assert_eqi(sim_state->gpr[6], ref_state->gpr[6]);
	log_assert_eqi(sim_state->gpr[7], ref_state->gpr[7]);

	log_assert_eqi(sim_state->gpr[8], ref_state->gpr[8]);
	log_assert_eqi(sim_state->gpr[9], ref_state->gpr[9]);
	log_assert_eqi(sim_state->gpr[10], ref_state->gpr[10]);
	log_assert_eqi(sim_state->gpr[11], ref_state->gpr[11]);
	log_assert_eqi(sim_state->gpr[12], ref_state->gpr[12]);
	log_assert_eqi(sim_state->gpr[13], ref_state->gpr[13]);
	log_assert_eqi(sim_state->gpr[14], ref_state->gpr[14]);
	log_assert_eqi(sim_state->gpr[15], ref_state->gpr[15]);

	log_assert_eqi(sim_state->gpr[16], ref_state->gpr[16]);
	log_assert_eqi(sim_state->gpr[17], ref_state->gpr[17]);
	log_assert_eqi(sim_state->gpr[18], ref_state->gpr[18]);
	log_assert_eqi(sim_state->gpr[19], ref_state->gpr[19]);
	log_assert_eqi(sim_state->gpr[20], ref_state->gpr[20]);
	log_assert_eqi(sim_state->gpr[21], ref_state->gpr[21]);
	log_assert_eqi(sim_state->gpr[22], ref_state->gpr[22]);
	log_assert_eqi(sim_state->gpr[23], ref_state->gpr[23]);
	log_assert_eqi(sim_state->gpr[24], ref_state->gpr[24]);

	log_assert_eqi(sim_state->gpr[25], ref_state->gpr[25]);
	log_assert_eqi(sim_state->gpr[26], ref_state->gpr[26]);
	log_assert_eqi(sim_state->gpr[27], ref_state->gpr[27]);
	log_assert_eqi(sim_state->gpr[28], ref_state->gpr[28]);
	log_assert_eqi(sim_state->gpr[29], ref_state->gpr[29]);
	log_assert_eqi(sim_state->gpr[30], ref_state->gpr[30]);
	log_assert_eqi(sim_state->gpr[31], ref_state->gpr[31]);
}

static void equiv_check_mems_named(const span_t sim_mem, const span_t ref_mem, const char* name) {
	log_assert_eqi(sim_mem.size, ref_mem.size);

	// Limit max unwinding amount for ESBMC
#ifndef ESBMC_MAX_MEM
	for (uint32_t i = 0; i < sim_mem.size; i++) {
#else
	log_assert_ltei(sim_mem.size, ESBMC_MAX_MEM);
	for (uint32_t i = 0; i < ESBMC_MAX_MEM; i++) {
		if (i == sim_mem.size) { break; }
#endif

		log_assert_fmt(
		    sim_mem.data[i] == ref_mem.data[i],
		    "(%s) sim_mem.data[0x%08x]: 0x%08x, ref_mem.data[0x%08x]: 0x%08x\n", name, i,
		    sim_mem.data[i], i, ref_mem.data[i]);
	}
}

void equiv_check_mems(const span_t sim_mem, const span_t ref_mem) {
	equiv_check_mems_named(sim_mem, ref_mem, "");
}

void equiv_check_instr_mems(const mips_core_t* sim_core, const mips_ref_core_t* ref_core) {
	equiv_check_mems_named(sim_core->config.instr_mem, ref_core->config.instr_mem, "instr_mem");
}

void equiv_check_data_mems(const mips_core_t* sim_core, const mips_ref_core_t* ref_core) {
	equiv_check_mems_named(sim_core->config.data_mem, ref_core->config.data_mem, "data_mem");
}

void equiv_check_configs(const mips_core_t* sim_core, const mips_ref_core_t* ref_core) {
	log_assert_eqi(sim_core->config.delay_slots, ref_core->config.delay_slots);
}

void equiv_check_cores(const mips_core_t* sim_core, const mips_ref_core_t* ref_core) {
	equiv_check_configs(sim_core, ref_core);
	equiv_check_gprs(sim_core, ref_core);
	equiv_check_data_mems(sim_core, ref_core);
}

void equiv_check_exceptions(
    const mips_exception_t* sim_exception, const mips_exception_t* ref_exception) {
	log_assert_eqi(sim_exception->raised, ref_exception->raised);
	if (!sim_exception->raised) { return; }

	log_assert_eqi(sim_exception->cause, ref_exception->cause);
	if (sim_exception->cause == MIPS_EXCP_ADDRL || sim_exception->cause == MIPS_EXCP_ADDRS) {
		log_assert_eqi(sim_exception->bad_v_addr, ref_exception->bad_v_addr);
	}
}

void equiv_check_retires_pic(
    const mips_retire_metadata_t* sim_retire, const mips_retire_metadata_t* ref_retire) {
	log_assert_eqi(sim_retire->active, ref_retire->active);
	log_assert_eqi(sim_retire->instruction, ref_retire->instruction);
	equiv_check_exceptions(&sim_retire->exception, &ref_retire->exception);
}

void equiv_check_retires(
    const mips_retire_metadata_t* sim_retire, const mips_retire_metadata_t* ref_retire) {
	log_assert_eqi(sim_retire->address, ref_retire->address);
	log_assert_eqi(sim_retire->instruction_number, ref_retire->instruction_number);
	equiv_check_retires_pic(sim_retire, ref_retire);
}
