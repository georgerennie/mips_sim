#ifndef _WRITEBACK_H_
#define _WRITEBACK_H_

#include <inttypes.h>
#include "arch_state.h"

typedef struct {
	uint8_t  reg;
	uint32_t value;
} writeback_bundle_t;

void writeback(mips_state_t* state, const writeback_bundle_t* bundle);
void writeback_bundle_init(writeback_bundle_t* bundle);

#endif
