#include <fstream>
#include <iostream>
#include <vector>
#include "assembler/assembler.hpp"
#include "core/core.h"
#include "include/argparse.hpp"
#include "util/log.h"

std::vector<uint8_t> assemble_file(const std::string& fn) {
	std::ifstream file(fn);
	if (!file.is_open()) { log_err_exit("Unable to open file %s\n", fn.c_str()); }

	return Assembler::assemble(file);
}

void run_sim(const std::string& fn) {
	auto instr_mem = assemble_file(fn);

	log_mem_hex(make_c_span(instr_mem));

	mips_core_t core;
	uint8_t     data_mem[100] = {0};
	mips_core_init(&core, make_c_span(instr_mem), make_c_span(data_mem));

	log_gprs_labelled(&core.state);

	for (size_t i = 0; i < 20; i++) {
		mips_core_cycle(&core);
		log_msg("$t0: 0x%08X\n", core.state.gpr[8]);
	}
}

int main(int argc, char* argv[]) {
	argparse::ArgumentParser program("mips_sim");

	program.add_argument("input_file").help("path to an assembly file to execute");

	try {
		program.parse_args(argc, argv);
	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::exit(EXIT_FAILURE);
	}

	auto input_file = program.get("input_file");
	run_sim(input_file);
}
