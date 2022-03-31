#ifndef _ARCH_STATE_H_
#define _ARCH_STATE_H_

#include <inttypes.h>

// Architectural State
typedef struct {
	// Program Counter
	uint32_t pc;

	// General Purpose Registers, index offset by 1 due to zero register
	uint32_t gpr[31];
} mips_state_t;

void mips_state_init(mips_state_t* state);

// TODO: Make these names more consistent
uint32_t gpr_read(const mips_state_t* state, uint8_t reg);
void     gpr_write(mips_state_t* state, uint8_t reg, uint32_t value);

#endif
