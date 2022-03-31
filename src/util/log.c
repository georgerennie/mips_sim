#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include "core/instruction.h"

inline int log_msg(const char *format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vprintf(format, args);
	va_end(args);
	return ret;
}

inline int log_dbg(const char *format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vprintf(format, args);
	va_end(args);
	return ret;
}

inline int log_err(const char *format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vfprintf(stderr, format, args);
	va_end(args);
	return ret;
}

void log_mem_hex(span_t mem) {
	const size_t row_bytes = 16, col_bytes = 2;

	for (size_t row = 0; row < mem.size; row += row_bytes) {
		log_msg("%08x: ", row);
		for (size_t col = 0; col < row_bytes; col += col_bytes) {
			for (size_t byte = 0; byte < col_bytes; byte++) {
				size_t addr = row + col + byte;
				if (addr < mem.size) { log_msg("%02x", *span_e(mem, row + col + byte)); }
			}
			log_msg(" ");
		}
		log_msg("\n");
	}
}

void log_gprs_labelled(mips_state_t *state) {
	const uint8_t gprs = 32, row_len = 5;

	for (uint8_t row = 0; row < gprs; row += row_len) {
		for (uint8_t col = 0; col < row_len; col++) {
			uint8_t addr = row + col;
			if (addr < gprs) {
				log_msg("%4s: %08x ", mips_reg_lookup[addr], gpr_read(state, addr));
			}
		}
		log_msg("\n");
	}
}
