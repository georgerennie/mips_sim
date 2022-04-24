#include "core.h"
#include "common/log.h"
#include "forwarding_unit.h"
#include "hazard_detection.h"

void mips_core_init(mips_core_t* core, mips_config_t config) {
	*core = (mips_core_t){
	    .state =
	        {
	            .pc  = TEXT_BASE_ADDR,
	            .gpr = {0},
	        },

	    .config             = config,
	    .cycle              = 0,
	    .instruction_number = 0,
	};
	pipeline_regs_init(&core->regs);
}

mips_retire_metadata_t mips_core_run_one(mips_core_t* core) {
	while (true) {
		mips_retire_metadata_t retire = mips_core_cycle(core);
		if (retire.active) { return retire; }
	}
}

mips_retire_metadata_t mips_core_cycle(mips_core_t* core) {
	log_dbg("Starting cycle %d\n", core->cycle);

	// ---------------- Pipeline stages ----------------------------------------
	mips_pipeline_regs_t next_regs;

	// Register writeback - Do this before anything else as in real hardware
	// WB would occur in the first half of the cycle, and ID in the second half
	writeback(&core->regs.mem_wb, &core->state);

	// Instruction Fetch
	next_regs.if_id = instruction_fetch(&core->state, core->config.instr_mem);

	// Decode / Register Fetch
	next_regs.id_ex = instruction_decode(&core->regs, &core->state);

	// Execute
	// Always updated forwarded values, this means that if EX is stalled, it
	// still reads the correct value out
	core->regs.id_ex.data_rs =
	    get_fwd_value(&core->regs, core->regs.id_ex.reg_rs, core->regs.id_ex.data_rs);
	core->regs.id_ex.data_rt =
	    get_fwd_value(&core->regs, core->regs.id_ex.reg_rt, core->regs.id_ex.data_rt);
	next_regs.ex_mem = execute(&core->regs);

	// Memory Access
	mips_pipeline_metadata_t retiring_stage = core->regs.mem_wb.metadata;
	next_regs.mem_wb                        = memory(&core->regs.ex_mem, core->config.data_mem);

	// ---------------- Pipeline register writeback ----------------------------

	// Detect hazards/branches
	hazard_flags_t hazards = detect_hazards(&core->regs, &next_regs, &core->config);

	if (hazards.flushes[MIPS_STAGE_IF]) {
		if_id_reg_init(&core->regs.if_id);
	} else if (!hazards.stalls[MIPS_STAGE_IF]) {
		core->regs.if_id = next_regs.if_id;
	}

	if (hazards.flushes[MIPS_STAGE_ID]) {
		id_ex_reg_init(&core->regs.id_ex);
	} else if (!hazards.stalls[MIPS_STAGE_ID]) {
		core->regs.id_ex = next_regs.id_ex;
	}

	if (hazards.flushes[MIPS_STAGE_EX]) {
		ex_mem_reg_init(&core->regs.ex_mem);
	} else if (!hazards.stalls[MIPS_STAGE_EX]) {
		core->regs.ex_mem = next_regs.ex_mem;
	}

	if (!hazards.stalls[MIPS_STAGE_MEM]) { core->regs.mem_wb = next_regs.mem_wb; }

	// Update PC
	if (!hazards.flushes[MIPS_STAGE_ID] && core->regs.id_ex.branch) {
		core->state.pc = core->regs.id_ex.branch_address;
	} else if (!hazards.stalls[MIPS_STAGE_IF]) {
		// TODO: reset pc on exception
		core->state.pc += 4;
	}

	const bool             retire_active = retiring_stage.active || retiring_stage.exception.raised;
	mips_retire_metadata_t retiring_instruction = {
	    .instruction = retiring_stage.instruction,
	    .address     = retiring_stage.address,
	    .active      = retire_active,
	    .exception   = retiring_stage.exception,

	    .cycle              = core->cycle,
	    .instruction_number = retire_active ? core->instruction_number++ : 0,
	};

	core->cycle++;
	return retiring_instruction;
}
