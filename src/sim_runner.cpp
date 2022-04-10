#include "sim_runner.hpp"
#include <iostream>
#include <vector>
#include "core/core.h"
#include "ref_core/ref_core.h"
#include "util/instruction.h"
#include "util/log.h"

void SimRunner::run(std::span<uint8_t> instr_mem) {
	if (config.compare) { return run_compare(instr_mem); }
	if (config.ref_core) { return run_reference(instr_mem); }
	return run_pipeline(instr_mem);
}

mips_config_t SimRunner::make_mips_config(
    std::span<uint8_t> instr_mem, std::span<uint8_t> data_mem) {
	mips_config_t mips_config;
	mips_config.delay_slots = config.delay_slots;
	mips_config.instr_mem   = make_c_span(instr_mem);
	mips_config.data_mem    = make_c_span(data_mem);
	return mips_config;
}

void SimRunner::log_instructions(
    std::deque<mips_retire_metadata_t>& instr_queue, size_t max_instrs,
    mips_retire_metadata_t new_instr) {
	if (new_instr.active) { instr_queue.push_back(new_instr); }
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

void SimRunner::run_pipeline(std::span<uint8_t> instr_mem) {
	mips_core_t          core;
	std::vector<uint8_t> data_mem(config.mem_size, 0);
	mips_core_init(&core, make_mips_config(instr_mem, data_mem));

	std::deque<mips_retire_metadata_t> last_instructions;

	for (size_t i = 0; i < 10000; i++) {
		if (!config.step) {
			mips_core_run_one(&core);
			continue;
		}

		clear_screen();
		const auto retire = mips_core_cycle(&core);

		log_instructions(last_instructions, 5, retire);
		log_pipeline_regs(&core.regs);
		log_gprs_labelled(&core.state);
		log_mem_hex(core.config.data_mem);
		wait_for_input();
	}

	log_gprs_labelled(&core.state);
	log_mem_hex(core.config.data_mem);
}

void SimRunner::run_reference(std::span<uint8_t> instr_mem) {
	mips_ref_core_t      ref_core;
	std::vector<uint8_t> data_mem(config.mem_size, 0);
	ref_core_init(&ref_core, make_mips_config(instr_mem, data_mem));

	std::deque<mips_retire_metadata_t> last_instructions;

	for (size_t i = 0; i < 10000; i++) {
		// TODO: Print current instruction/addr etc in single step mode
		if (config.step) { clear_screen(); }
		const auto retire = ref_core_cycle(&ref_core);
		if (!config.step) { continue; }

		log_instructions(last_instructions, 5, retire);
		log_gprs_labelled(&ref_core.state);
		log_mem_hex(ref_core.config.data_mem);
		wait_for_input();
	}

	log_gprs_labelled(&ref_core.state);
	log_mem_hex(ref_core.config.data_mem);
}

void SimRunner::run_compare(std::span<uint8_t> instr_mem) {
	mips_core_t     core;
	mips_ref_core_t ref_core;

	std::vector<uint8_t> pipeline_data_mem(config.mem_size, 0);
	std::vector<uint8_t> ref_data_mem(config.mem_size, 0);

	mips_core_init(&core, make_mips_config(instr_mem, pipeline_data_mem));
	ref_core_init(&ref_core, make_mips_config(instr_mem, ref_data_mem));

	// TODO: Define how compare works, for both stepped and normal mode
	// for (size_t i = 0; i < 10000; i++) {
	//     const auto core_retire = mips_core_run_one(&core);
	//     const auto ref_retire  = ref_core_cycle(&ref_core);
	// }
}

inline void SimRunner::wait_for_input() { std::cin.get(); }
inline void SimRunner::clear_screen() { log_msg("\033[2J\033[H"); }
