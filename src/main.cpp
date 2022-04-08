#include <fstream>
#include <iostream>
#include <vector>
#include "assembler/assembler.hpp"
#include "core/core.h"
#include "include/argparse.hpp"
#include "ref_core/ref_core.h"
#include "util/log.h"

std::vector<uint8_t> assemble_file(const std::string& fn) {
	std::ifstream file(fn);
	if (!file.is_open()) { log_err_exit("Unable to open file \"%s\"\n", fn.c_str()); }

	return Assembler::assemble(file);
}

void run_sim(const std::string& fn, bool delay_slots, bool use_ref_core) {
	auto    instr_mem     = assemble_file(fn);
	uint8_t data_mem[100] = {0};

	log_mem_hex(make_c_span(instr_mem));

	if (!use_ref_core) {
		mips_core_t core;
		mips_core_init(&core, make_c_span(instr_mem), make_c_span(data_mem), delay_slots);

		for (size_t i = 0; i < 20; i++) { mips_core_cycle(&core); }
		log_gprs_labelled(&core.state);
		log_mem_hex(make_c_span(data_mem));
	} else {
		mips_ref_core_t ref_core;
		ref_core_init(&ref_core, make_c_span(instr_mem), make_c_span(data_mem), delay_slots);

		for (size_t i = 0; i < 20; i++) { ref_core_cycle(&ref_core); }
		log_gprs_labelled(&ref_core.state);
		log_mem_hex(make_c_span(data_mem));
	}
}

int main(int argc, char* argv[]) {
	argparse::ArgumentParser program("mips_sim");

	program.add_argument("input_file").help("path to an assembly file to execute");
	program.add_argument("-d", "--delay-slots")
	    .help("enable delay slot emulation")
	    .default_value(false)
	    .implicit_value(true);
	program.add_argument("-r", "--ref-core")
	    .help("run simulation using single cycle reference model")
	    .default_value(false)
	    .implicit_value(true);

	try {
		program.parse_args(argc, argv);
	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::exit(EXIT_FAILURE);
	}

	const auto input_file  = program.get("input_file");
	const auto delay_slots = program.get<bool>("--delay-slots");
	const auto ref_core    = program.get<bool>("--ref-core");
	run_sim(input_file, delay_slots, ref_core);
}
