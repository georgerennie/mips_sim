#include <sstream>
#include "assembler/assembler.hpp"
#include "core/core.h"
#include "util/log.h"

int main() {
	const auto assembly =
	    "addiu $t0, $zero, 2\n"
	    "addiu $t1, $t0, 3\n"
	    "addu $t2, $t1, $t0\n";

	std::istringstream assembly_stream(assembly);
	auto               instr_mem = Assembler::assemble(assembly_stream);

	// instr_mem[3] = 0;
	log_mem_hex({(uint8_t*) instr_mem.data(), instr_mem.size() * 4});

	mips_core_t core;
	uint8_t     data_mem[100] = {0};
	mips_core_init(&core, make_c_span(instr_mem), make_c_span(data_mem));

	log_gprs_labelled(&core.state);
	log_msg("Stopped on trap 0x%08X\n", mips_core_run(&core));
	// for (size_t i = 0; i < 15; i++) {
	//     log_msg("Trap 0x%08X\n", mips_core_cycle(&core));
	// }
	log_gprs_labelled(&core.state);
}
