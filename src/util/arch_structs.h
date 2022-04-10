#ifndef ARCH_STATE_H
#define ARCH_STATE_H

#include <inttypes.h>
#include <stdbool.h>
#include "util/util.h"

// -------- Architectural State ------------------------------------------------

typedef struct {
	// Program Counter
	uint32_t pc;

	// General Purpose Registers, 0 should always be 0
	uint32_t gpr[32];
} mips_state_t;

// -------- Core Configuration -------------------------------------------------

typedef struct {
	bool delay_slots;

	span_t instr_mem;
	span_t data_mem;
} mips_config_t;

// -------- Retire Metadata ----------------------------------------------------

// This is included for observability, not operation of the pipeline
// TODO: This is all being passed down the pipeline at the moment, but a lot of
// it is only generated in WB, this is unecessary
typedef struct {
	uint32_t instruction;
	uint32_t address;

	bool active; // True if this is a real instruction, rather than pipeline stall/flush

	// Sequentially retiring instructions must have sequential instruction
	// numbers. Starting at 0
	uint32_t instruction_number;

	// Clock cycle that just retired, starting at 0
	uint32_t cycle;
} mips_retire_metadata_t;

#endif
