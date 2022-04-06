#ifndef ESBMC_UTIL_H
#define ESBMC_UTIL_H

#include <inttypes.h>
#include "log.h"

#ifdef ESBMC

uint8_t  nondet_u8();
int8_t   nondet_i8();
uint16_t nondet_u16();
int16_t  nondet_i16();
uint32_t nondet_u32();
int32_t  nondet_i32();

	#define esbmc_assume(CLAUSE)   __ESBMC_assume(CLAUSE)
	#define esbmc_restrict(CLAUSE) __ESBMC_assume(CLAUSE)

#else

	#define esbmc_assume(CLAUSE) log_assert(CLAUSE)
	#define esbmc_restrict(CLAUSE) \
		{}

#endif
#endif
