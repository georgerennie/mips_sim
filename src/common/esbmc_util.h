#ifndef ESBMC_UTIL_H
#define ESBMC_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"

#ifdef ESBMC

	#include <inttypes.h>

uint8_t  nondet_u8();
int8_t   nondet_i8();
uint16_t nondet_u16();
int16_t  nondet_i16();
uint32_t nondet_u32();
int32_t  nondet_i32();
    #define nondet_reg() (nondet_u8() & 0x1F)

    #define esbmc_assume(CLAUSE)   __ESBMC_assume(CLAUSE)
    #define esbmc_restrict(CLAUSE) __ESBMC_assume(CLAUSE)

#else

	#define esbmc_assume(CLAUSE) log_assert(CLAUSE)
	#define esbmc_restrict(CLAUSE) \
		{}

#endif

#ifdef __cplusplus
}
#endif

#endif
