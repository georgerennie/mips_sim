#ifndef ARCH_H
#define ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
#include "util.h"

// -------- Fixed width integers -----------------------------------------------

#ifdef ESBMC
// NOTE: ESBMC currently doesn't properly align _ExtInt fields in structs, so
// this must be set to uint8_t for correct compilation currently
// This has been raised as an issue at https://github.com/esbmc/esbmc/issues/673
typedef uint8_t  mips_reg_idx_t;
typedef uint8_t  mips_shamt_t;
typedef uint32_t mips_j_address_t;
#else
typedef unsigned _ExtInt(5) mips_reg_idx_t;
typedef unsigned _ExtInt(5) mips_shamt_t;
typedef unsigned _ExtInt(26) mips_j_address_t;
#endif

// -------- Architectural State ------------------------------------------------

typedef struct {
	// Program Counter
	uint32_t pc;

	// General Purpose Registers, 0 should always be 0
	uint32_t gpr[32];
} mips_state_t;

void               log_gprs_labelled(const mips_state_t* state);
extern const char* mips_reg_lookup[32];

// -------- Core Configuration -------------------------------------------------

typedef struct {
	bool delay_slots;

	span_t instr_mem;
	span_t data_mem;
} mips_config_t;

// -------- Retire Metadata ----------------------------------------------------

// Only a subset of MIPS interrupts are implemented
typedef enum {
	MIPS_EXCP_ADDRL = 4,  // Incorrect load/instruction fetch address
	MIPS_EXCP_ADDRS = 5,  // Incorrect store address
	MIPS_EXCP_BREAK = 9,  // Breakpoint
	MIPS_EXCP_RI    = 10, // Reserved instruction
	MIPS_EXCP_OVF   = 12, // Overflow
} ATTR_PACKED mips_exception_cause_t;

const char* mips_exception_name(mips_exception_cause_t exception);

// Coprocessor 0 is used for tracking exceptions. Only a limited number of its
// registers are stored here, and as the processor only implements user mode,
// they cannot actually be read by the program and thus are only for debugging
// when the simulator hits an exception
typedef struct {
	bool                   raised;
	uint32_t               bad_v_addr; // reg 8
	mips_exception_cause_t cause;      // A subset of the cause register (reg 13)
} mips_exception_t;

// This is included for observability, not operation of the pipeline
typedef struct {
	uint32_t instruction;
	uint32_t address;

	// True if this is a real instruction, rather than pipeline stall/flush
	bool active;

	// Sequentially retiring instructions must have sequential instruction
	// numbers. Starting at 0
	uint32_t instruction_number;

	// Clock cycle that just retired, starting at 0
	uint32_t cycle;

	// This should contain details of any exception raised by this instruction
	mips_exception_t exception;
} mips_retire_metadata_t;

#ifdef __cplusplus
}
#endif

#endif
