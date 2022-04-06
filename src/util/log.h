#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core/arch_state.h"
#include "util.h"
#include <inttypes.h>

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

#define log_assert_cond(A, COND, B, FMT) \
	log_assert_fmt((A) COND (B), #A " = " FMT ", " #B " = " FMT "\n", A, B)
#define log_assert_condi(A, COND, B) \
	log_assert_cond(A, COND, B, "%" PRIu32)

#define log_assert_eq(A, B, FMT) log_assert_cond(A, ==, B, FMT)
#define log_assert_neq(A, B, FMT) log_assert_cond(A, !=, B, FMT)
#define log_assert_gt(A, B, FMT) log_assert_cond(A, >, B, FMT)
#define log_assert_gte(A, B, FMT) log_assert_cond(A, >=, B, FMT)
#define log_assert_lt(A, B, FMT) log_assert_cond(A, <, B, FMT)
#define log_assert_lte(A, B, FMT) log_assert_cond(A, <=, B, FMT)

// Variants with format specifiers for u32s already
#define log_assert_eqi(A, B) log_assert_condi(A, ==, B)
#define log_assert_neqi(A, B) log_assert_condi(A, !=, B)
#define log_assert_gti(A, B) log_assert_condi(A, >, B)
#define log_assert_gtei(A, B) log_assert_condi(A, >=, B)
#define log_assert_lti(A, B) log_assert_condi(A, <, B)
#define log_assert_ltei(A, B) log_assert_condi(A, <=, B)

// Log printf style format
int log_err(const char *format, ...);
int log_msg(const char *format, ...);
int log_dbg(const char *format, ...);
int log_dbgi(const char *format, ...); // Indented by one tab

void log_mem_hex(span_t mem);
void log_gprs_labelled(mips_state_t *state);

#ifdef __cplusplus
}
#endif

#endif
