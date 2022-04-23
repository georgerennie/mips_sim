#ifndef SIM_RUNNER_HPP
#define SIM_RUNNER_HPP

#include <cstdint>
#include <deque>
#include <optional>
#include <span>
#include "common/arch.h"

class SimRunner {
public:
	struct Config {
		bool step;        // Single step through simulation
		bool quiet;       // Don't print output
		bool ref_core;    // Execute on reference core instead of pipelined core
		bool compare;     // Compare outputs of reference core and pipelined core
		bool delay_slots; // Emulate delay slots

		uint32_t mem_size; // Size of data memory in bytes
	};

	SimRunner(const Config&& config) : config(config) {}

	void run(std::span<uint8_t> instr_mem);

private:
	Config config;

	mips_config_t make_mips_config(std::span<uint8_t> instr_mem, std::span<uint8_t> data_mem) const;

	static void log_instructions(
	    std::deque<mips_retire_metadata_t>& instr_queue, size_t max_instrs,
	    std::optional<mips_retire_metadata_t> new_instr);

	template <typename T>
	static void log_core_state(
	    const T& core, std::deque<mips_retire_metadata_t>& instr_queue,
	    const std::optional<mips_retire_metadata_t>& new_instr) {
		log_instructions(instr_queue, 5, new_instr);
		log_gprs_labelled(&core.state);
		log_mem_hex(core.config.data_mem);
	}

	static void log_exception(mips_retire_metadata_t& metadata);

	void run_pipeline(std::span<uint8_t> instr_mem) const;
	void run_reference(std::span<uint8_t> instr_mem) const;
	void run_compare(std::span<uint8_t> instr_mem) const;

	static void wait_for_input();
	static void clear_screen();
};

#endif
