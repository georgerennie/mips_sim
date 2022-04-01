#include <sstream>
#include "assembler/assembler.hpp"
#include "core/core.h"
#include "ref_core/ref_core.h"
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

	mips_core_t     core;
	mips_ref_core_t ref_core;

	uint8_t data_mem[100]     = {0};
	uint8_t ref_data_mem[100] = {0};

	mips_core_init(&core, make_c_span(instr_mem), make_c_span(data_mem));
	ref_core_init(&ref_core, make_c_span(instr_mem), make_c_span(ref_data_mem));

	log_msg("Pipelined start regs:\n");
	log_gprs_labelled(&core.state);
	log_msg("Reference start regs:\n");
	log_gprs_labelled(&ref_core.state);

	log_msg("Stopped pipelined core on trap 0x%08X\n", mips_core_run(&core));
	log_msg("Stopped ref core on trap 0x%08X\n", ref_core_run(&ref_core));

	// for (size_t i = 0; i < 15; i++) {
	//     log_msg("Trap 0x%08X\n", mips_core_cycle(&core));
	// }

	log_msg("Pipelined end regs:\n");
	log_gprs_labelled(&core.state);
	log_msg("Reference end regs:\n");
	log_gprs_labelled(&ref_core.state);
}
