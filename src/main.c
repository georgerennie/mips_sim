#include "core/core.h"
#include "esbmc_util.h"

int main() {
	mips_core_t core;
	uint32_t    instr_mem[10] = {0};
	uint8_t     data_mem[4];
	mips_core_init(&core, instr_mem, sizeof(instr_mem), ARRAY_TO_SPAN(data_mem));

	for (uint8_t i = 1; i < 5; i++) {
#ifdef __ESBMC
		instr_mem[i] = nondet_u32();
#else
		instr_mem[i] = (uint32_t) MIPS_OPC_ADDI << 26 | i << 16 | (i * 3);
#endif
	}

	print_gprs(&core.state);
	for (int i = 0; i < 10; i++) { mips_core_cycle(&core); }
	print_gprs(&core.state);
}
