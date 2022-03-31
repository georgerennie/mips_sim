#include <sstream>
#include "assembler/assembler.hpp"
#include "core/core.h"
#include "util/log.h"

int main() {
	const auto assembly =
	    "addiu $t0, $zero, 12\n"
	    "addiu $t1, $zero, 12\n"
	    "addiu $t2, $zero, 12\n"
	    "addiu $t3, $zero, 12\n"
	    "addiu $t4, $zero, 12\n"
	    "addiu $t5, $zero, 12\n";

	std::istringstream assembly_stream(assembly);
	auto               instr_mem = Assembler::assemble(assembly_stream);

	log_mem_hex({(uint8_t*) instr_mem.data(), instr_mem.size() * 4});

	mips_core_t core;
	uint8_t     data_mem[100] = {0};
	mips_core_init(&core, instr_mem.data(), instr_mem.size(), make_c_span(data_mem));

	log_gprs_labelled(&core.state);
	for (int i = 0; i < 10; i++) { mips_core_cycle(&core); }
	log_gprs_labelled(&core.state);
}
