#include <fstream>
#include <iostream>
#include "assembler.hpp"
#include "include/argparse.hpp"
#include "sim_runner.hpp"
#include "util/log.h"

std::vector<uint8_t> assemble_file(const std::string& fn) {
	std::ifstream file(fn);
	if (!file.is_open()) { log_err_exit("Unable to open file \"%s\"\n", fn.c_str()); }
	return Assembler::assemble(file);
}

int main(int argc, char* argv[]) {
	argparse::ArgumentParser program("mips_sim");

	program.add_argument("input_file").help("path to an assembly file to execute");

	program.add_argument("-s", "--step")
	    .help("single step through the simulation")
	    .default_value(false)
	    .implicit_value(true);

	program.add_argument("-r", "--ref-core")
	    .help("run simulation using single cycle reference model")
	    .default_value(false)
	    .implicit_value(true);

	program.add_argument("-c", "--compare")
	    .help("run both models and compare the output")
	    .default_value(false)
	    .implicit_value(true);

	program.add_argument("-d", "--delay-slots")
	    .help("enable delay slot emulation")
	    .default_value(false)
	    .implicit_value(true);

	program.add_argument("-m", "--mem-size")
	    .help("data memory size in bytes")
	    .default_value(512U)
	    .scan<'u', uint32_t>();

	try {
		program.parse_args(argc, argv);
	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::exit(EXIT_FAILURE);
	}

	SimRunner::Config config;
	config.step        = program.get<bool>("--step");
	config.ref_core    = program.get<bool>("--ref-core");
	config.compare     = program.get<bool>("--compare");
	config.delay_slots = program.get<bool>("--delay-slots");
	config.mem_size    = program.get<uint32_t>("--mem-size");

	SimRunner sim(std::move(config));
	auto      instr_mem = assemble_file(program.get("input_file"));
	sim.run(instr_mem);
}
