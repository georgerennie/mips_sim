#include <sstream>
#include "assembler/assembler.hpp"
#include "core/core.h"
#include "util/log.h"

int main() {
	const auto assembly =
	    "addiu $t0, $zero, 0\n"
	    "main:\n"
	    "addiu $t0, $t0, -1\n"
	    "j main\n";

	std::istringstream assembly_stream(assembly);
	auto               instr_mem = Assembler::assemble(assembly_stream);

	// instr_mem[3] = 0;
	log_mem_hex(make_c_span(instr_mem));

	mips_core_t core;

	uint8_t data_mem[100] = {0};

	mips_core_init(&core, make_c_span(instr_mem), make_c_span(data_mem));

	log_gprs_labelled(&core.state);

	for (size_t i = 0; i < 20; i++) {
		mips_core_cycle(&core);
		log_msg("$t0: 0x%08X\n", core.state.gpr[8]);
	}
}
