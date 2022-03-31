#ifndef _LOG_H_
#define _LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "core/arch_state.h"
#include "util.h"

#ifndef __ESBMC
	#include <stdio.h>
	#include <stdlib.h>

	#define log_assert_fmt(COND, ...)                                                 \
		{                                                                             \
			if (!(COND)) {                                                            \
				log_err("ASSERTION FAILED (%s:%d): " #COND "\n", __FILE__, __LINE__); \
				log_err(__VA_ARGS__);                                                 \
				exit(EXIT_FAILURE);                                                   \
			}                                                                         \
		}

#else
	#include <assert.h>
	#define log_assert_fmt(COND, ...) assert(COND)
#endif

#define log_assert(COND)     log_assert_fmt(COND, "")
#define log_assert_fail(...) log_assert_fmt(0, __VA_ARGS__)

// Log printf style format
int log_err(const char *format, ...);
int log_msg(const char *format, ...);
int log_dbg(const char *format, ...);

void log_mem_hex(span_t mem);
void log_gprs_labelled(mips_state_t *state);

#ifdef __cplusplus
}
#endif

#endif
