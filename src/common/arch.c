#include "arch.h"
#include "log.h"

const char* mips_excp_cause_to_str(mips_exception_cause_t exception) {
	switch (exception) {
		case MIPS_EXCP_ADDRL: return "addrl";
		case MIPS_EXCP_ADDRS: return "addrs";
		case MIPS_EXCP_BREAK: return "break";
		case MIPS_EXCP_RI: return "ri";
		case MIPS_EXCP_OVF: return "ovf";
		default: return "";
	}
}

const char* mips_exception_name(mips_exception_cause_t exception) {
	switch (exception) {
		case MIPS_EXCP_ADDRL: return "invalid load address";
		case MIPS_EXCP_ADDRS: return "invalid store address";
		case MIPS_EXCP_BREAK: return "breakpoint";
		case MIPS_EXCP_RI: return "reserved instruction";
		case MIPS_EXCP_OVF: return "overflow";
		default: return "";
	}
}

#ifdef ESBMC
void log_gprs_labelled(const mips_state_t* state) {}
#else

void log_gprs_labelled(const mips_state_t* state) {
	const uint8_t gprs = 32, row_len = 6;

	for (uint8_t row = 0; row < gprs; row += row_len) {
		for (uint8_t col = 0; col < row_len; col++) {
			uint8_t addr = row + col;
			if (addr < gprs) { log_msg("%4s: %08x ", mips_reg_lookup[addr], state->gpr[addr]); }
		}
		log_msg("\n");
	}
}

#endif

const char* mips_reg_lookup[32] = {"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
                                   "t0",   "t1", "t2", "t3", "t4", "t5", "t6", "t7",
                                   "s0",   "s1", "s2", "s3", "s4", "s4", "s6", "s7",
                                   "t8",   "t9", "k0", "k1", "gp", "sp", "fp", "ra"};
