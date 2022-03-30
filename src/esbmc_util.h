#ifndef _ESBMC_UTIL_H_
#define _ESBMC_UTIL_H_
#include <assert.h>
#include <inttypes.h>

#ifdef __ESBMC

uint8_t  nondet_u8();
int8_t   nondet_i8();
uint16_t nondet_u16();
int16_t  nondet_i16();
uint32_t nondet_u32();
int32_t  nondet_i32();

#else

	#define __ESBMC_assume(CLAUSE) assert(CLAUSE)

#endif
#endif
