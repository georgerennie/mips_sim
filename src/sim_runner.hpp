#ifndef SIM_RUNNER_HPP
#define SIM_RUNNER_HPP

#include <cstdint>
#include <deque>
#include <optional>
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

	void log_instructions(
	    std::deque<mips_retire_metadata_t>& instr_queue, size_t max_instrs,
	    std::optional<mips_retire_metadata_t> new_instr);

	template <typename T>
	void log_core_state(const T& core, std::deque<mips_retire_metadata_t>& instr_queue, std::optional<mips_retire_metadata_t> new_instr) {
		log_instructions(instr_queue, 5, new_instr);
		log_gprs_labelled(&core.state);
		log_mem_hex(core.config.data_mem);
	}

	void run_pipeline(std::span<uint8_t> instr_mem);
	void run_reference(std::span<uint8_t> instr_mem);
	void run_compare(std::span<uint8_t> instr_mem);

	void wait_for_input();
	void clear_screen();
};

#endif
