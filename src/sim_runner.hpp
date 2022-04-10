#ifndef SIM_RUNNER_HPP
#define SIM_RUNNER_HPP

#include <cstdint>
#include <span>
#include "util/arch_structs.h"

class SimRunner {
public:
	struct Config {
		bool step;        // Single step through simulation
		bool ref_core;    // Execute on reference core instead of pipelined core
		bool compare;     // Compare outputs of reference core and pipelined core
		bool delay_slots; // Emulate delay slots

		uint32_t mem_size; // Size of data memory in bytes
	};

	SimRunner(const Config&& config) : config(config) {}

	void run(std::span<uint8_t> instr_mem);

private:
	Config config;

	mips_config_t make_mips_config(std::span<uint8_t> instr_mem, std::span<uint8_t> data_mem);

	void run_pipeline(std::span<uint8_t> instr_mem);
	void run_reference(std::span<uint8_t> instr_mem);
	void run_compare(std::span<uint8_t> instr_mem);

	void wait_for_input();
	void clear_screen();
};

#endif
