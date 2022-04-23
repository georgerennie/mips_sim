#include "sim_runner.hpp"
#include <iostream>
#include <vector>
#include "common/instruction.h"
#include "common/log.h"
#include "core/core.h"
#include "equiv_check/equiv_check.h"
#include "ref_core/ref_core.h"

void SimRunner::run(std::span<uint8_t> instr_mem) {
	if (config.compare) { return run_compare(instr_mem); }
	if (config.ref_core) { return run_reference(instr_mem); }
	return run_pipeline(instr_mem);
}

mips_config_t SimRunner::make_mips_config(
    std::span<uint8_t> instr_mem, std::span<uint8_t> data_mem) const {
	mips_config_t mips_config;
	mips_config.delay_slots = config.delay_slots;
	mips_config.instr_mem   = make_c_span(instr_mem);
	mips_config.data_mem    = make_c_span(data_mem);
	return mips_config;
}

void SimRunner::log_instructions(
    std::deque<mips_retire_metadata_t>& instr_queue, size_t max_instrs,
    std::optional<mips_retire_metadata_t> new_instr) {
	if (new_instr && new_instr->active) { instr_queue.push_back(*new_instr); }
	if (instr_queue.size() > max_instrs) { instr_queue.pop_front(); }

	const auto blank_instrs = max_instrs - instr_queue.size();
	for (size_t i = 0; i < max_instrs; i++) {
		log_msg("%c ", i == max_instrs - 1 ? '>' : ' ');

		if (i < blank_instrs) {
			log_msg("...\n");
			continue;
		}

		const auto& instr = instr_queue[i - blank_instrs];
		log_msg(
		    "0x%08x (%d): %s\n", instr.address, instr.instruction_number,
		    mips_instr_name(instr.instruction));
	}
}

void SimRunner::log_exception(mips_retire_metadata_t& metadata) {
	log_assert(metadata.exception.raised);
	const auto cause = metadata.exception.cause;

	log_msg("Halted due to exception:\n");
	log_msg("    Cause: %s\n", mips_exception_name(cause));
	log_msg("    EPC: 0x%08x\n", metadata.address);
	if (cause == MIPS_EXCP_ADDRL || cause == MIPS_EXCP_ADDRS) {
		log_msg("    Bad VAddr: 0x%08x\n", metadata.exception.bad_v_addr);
	}
	log_msg("%d instructions executed in %d cycles\n", metadata.instruction_number, metadata.cycle);
}

void SimRunner::run_pipeline(std::span<uint8_t> instr_mem) const {
	mips_core_t          core;
	std::vector<uint8_t> data_mem(config.mem_size, 0);
	mips_core_init(&core, make_mips_config(instr_mem, data_mem));
	mips_retire_metadata_t retire = {};

	if (!config.step) {
		while (!retire.exception.raised) { retire = mips_core_run_one(&core); }
		if (!config.quiet) {
			log_gprs_labelled(&core.state);
			log_mem_hex(core.config.data_mem);
			log_exception(retire);
		}
		return;
	}

	std::deque<mips_retire_metadata_t> last_instructions;
	clear_screen();
	log_pipeline_regs(&core.regs);
	log_core_state(core, last_instructions, std::nullopt);
	while (!retire.exception.raised) {
		wait_for_input();
		clear_screen();
		retire = mips_core_cycle(&core);
		log_pipeline_regs(&core.regs);
		log_core_state(core, last_instructions, retire);
	}
	log_exception(retire);
}

void SimRunner::run_reference(std::span<uint8_t> instr_mem) const {
	mips_ref_core_t      ref_core;
	std::vector<uint8_t> data_mem(config.mem_size, 0);
	ref_core_init(&ref_core, make_mips_config(instr_mem, data_mem));
	mips_retire_metadata_t retire = {};

	if (!config.step) {
		while (!retire.exception.raised) { retire = ref_core_cycle(&ref_core); }
		if (!config.quiet) {
			log_gprs_labelled(&ref_core.state);
			log_mem_hex(ref_core.config.data_mem);
			log_exception(retire);
		}
		return;
	}

	std::deque<mips_retire_metadata_t> last_instructions;
	clear_screen();
	log_core_state(ref_core, last_instructions, std::nullopt);
	while (!retire.exception.raised) {
		wait_for_input();
		clear_screen();
		retire = ref_core_cycle(&ref_core);
		log_core_state(ref_core, last_instructions, retire);
	}
	log_exception(retire);
}

void SimRunner::run_compare(std::span<uint8_t> instr_mem) const {
	mips_core_t     core;
	mips_ref_core_t ref_core;

	std::vector<uint8_t> pipeline_data_mem(config.mem_size, 0);
	std::vector<uint8_t> ref_data_mem(config.mem_size, 0);

	mips_core_init(&core, make_mips_config(instr_mem, pipeline_data_mem));
	ref_core_init(&ref_core, make_mips_config(instr_mem, ref_data_mem));
	equiv_check_cores(&core, &ref_core);

	mips_retire_metadata_t retire = {};

	if (!config.step) {
		while (!retire.exception.raised) {
			retire                = mips_core_run_one(&core);
			const auto ref_retire = ref_core_cycle(&ref_core);

			if (!config.quiet && retire.exception.raised) {
				log_gprs_labelled(&core.state);
				log_mem_hex(core.config.data_mem);
				log_exception(retire);
			}
			equiv_check_retires(&retire, &ref_retire);
			equiv_check_gprs(&core, &ref_core);
		}
		equiv_check_cores(&core, &ref_core);
		return;
	}

	std::deque<mips_retire_metadata_t> last_instructions;
	clear_screen();
	log_pipeline_regs(&core.regs);
	log_core_state(core, last_instructions, std::nullopt);
	while (!retire.exception.raised) {
		wait_for_input();
		clear_screen();
		retire = mips_core_cycle(&core);
		log_pipeline_regs(&core.regs);
		log_core_state(core, last_instructions, retire);

		if (retire.exception.raised || retire.active) {
			const auto ref_retire = ref_core_cycle(&ref_core);
			equiv_check_retires(&retire, &ref_retire);
			equiv_check_gprs(&core, &ref_core);
		}
	}
	log_exception(retire);
	equiv_check_cores(&core, &ref_core);
}

inline void SimRunner::wait_for_input() { std::cin.get(); }
inline void SimRunner::clear_screen() { log_msg("\033[2J\033[H"); }
