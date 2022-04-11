#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"

#ifdef ESBMC

inline int log_err(const char *format, ...) { return nondet_int(); }
inline int log_msg(const char *format, ...) { return nondet_int(); }
inline int log_dbg(const char *format, ...) { return nondet_int(); }
inline int log_dbgi(const char *format, ...) { return nondet_int(); }

void log_mem_hex(span_t mem) {}
void log_gprs_labelled(const mips_state_t *state) {}

#else

inline int log_msg(const char *format, ...) {
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

inline int log_err_exit(const char *format, ...) {
	va_list args;
	va_start(args, format);
	fprintf(stderr, "(ERROR) ");
	vfprintf(stderr, format, args);
	va_end(args);
	exit(EXIT_FAILURE);
}

	#ifdef DEBUG_PRINT

inline int log_dbg(const char *format, ...) {
	va_list args;
	va_start(args, format);
	int ret = vprintf(format, args);
	va_end(args);
	return ret;
}

inline int log_dbgi(const char *format, ...) {
	va_list args;
	va_start(args, format);
	int ret = printf("    ");
	ret |= vprintf(format, args);
	va_end(args);
	return ret;
}

	#else

inline int log_dbg(const char *format, ...) {
	(void) format;
	return 0;
}

inline int log_dbgi(const char *format, ...) {
	(void) format;
	return 0;
}

	#endif

void log_mem_hex(span_t mem) {
	const size_t row_bytes = 32, col_bytes = 2;

	log_msg("%10c", ' ');
	for (size_t col = 0; col < row_bytes; col += col_bytes) { log_msg("%*x ", col_bytes * 2, col); }
	log_msg("\n");

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

void log_gprs_labelled(const mips_state_t *state) {
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
