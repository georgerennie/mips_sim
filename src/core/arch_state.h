#ifndef _ARCH_STATE_H_
#define _ARCH_STATE_H_

#include <inttypes.h>

// Architectural State
typedef struct {
	// Program Counter
	uint32_t pc;

	// General Purpose Registers, 0 should always be 0
	uint32_t gpr[32];
} mips_state_t;

// Traps generated by the core
typedef enum {
	MIPS_TRAP_NONE = 0,

	MIPS_TRAP_UNKNOWN_INSTR    = 1 << 0,
	MIPS_TRAP_INSTR_PAGE_FAULT = 1 << 1,
} mips_trap_t;

void mips_state_init(mips_state_t* state);

// TODO: Make these names more consistent
uint32_t gpr_read(const mips_state_t* state, uint8_t reg);
void     gpr_write(mips_state_t* state, uint8_t reg, uint32_t value);

#endif
