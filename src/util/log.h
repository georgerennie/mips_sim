#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core/arch_state.h"
#include "util.h"

#if defined(ESBMC)
	#include <assert.h>
	#define log_assert_fmt(COND, ...) assert(COND)

#elif defined(NDEBUG)
	#define log_assert_fmt(COND, ...) \
		{}

#elif defined(UNIT_TEST)
	#include <stdio.h>
	#include "unit/test.h"

	#define log_assert_fmt(COND, ...)                                                 \
		{                                                                             \
			if (!(COND)) {                                                            \
				log_err("ASSERTION FAILED (%s:%d): " #COND "\n", __FILE__, __LINE__); \
				log_err(__VA_ARGS__);                                                 \
				test_fail();                                                          \
			}                                                                         \
		}

#else
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
