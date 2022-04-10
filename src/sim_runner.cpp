#include "sim_runner.hpp"
#include <iostream>
#include <vector>
#include "core/core.h"
#include "ref_core/ref_core.h"
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

void SimRunner::run_pipeline(std::span<uint8_t> instr_mem) {
	mips_core_t          core;
	std::vector<uint8_t> data_mem(config.mem_size, 0);
	mips_core_init(&core, make_mips_config(instr_mem, data_mem));

	// TODO: Add single steppping to this and run_reference, complete run_compare
	for (size_t i = 0; i < 10000; i++) { mips_core_run_one(&core); }

	log_gprs_labelled(&core.state);
	log_mem_hex(core.config.data_mem);
}

void SimRunner::run_reference(std::span<uint8_t> instr_mem) {
	mips_ref_core_t      ref_core;
	std::vector<uint8_t> data_mem(config.mem_size, 0);
	ref_core_init(&ref_core, make_mips_config(instr_mem, data_mem));

	for (size_t i = 0; i < 10000; i++) { ref_core_run_one(&ref_core); }

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
}

inline void SimRunner::wait_for_input() { std::cin.get(); }
inline void SimRunner::clear_screen() { log_msg("\033[2J\033[H"); }
