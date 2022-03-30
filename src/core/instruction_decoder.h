#ifndef _INSTRUCTION_DECODER_H_
#define _INSTRUCTION_DECODER_H_

#include "arch_state.h"
#include "instruction.h"

typedef enum {
	ALU_OP_NOP,

	ALU_OP_ADD,
	ALU_OP_SUB,

	ALU_OP_AND,
	ALU_OP_OR,
	ALU_OP_XOR,
	ALU_OP_NOR,
} alu_op_t;

typedef enum {
	MEM_ACCESS_NONE,
	MEM_ACCESS_READ_UNSIGNED,
	MEM_ACCESS_READ_SIGNED,
	MEM_ACCESS_WRITE,
} mem_access_type_t;

typedef struct {
	// Instruction format
	mips_instr_format_t format;

	alu_op_t alu_op;               // ALU Operation
	uint32_t alu_arg_a, alu_arg_b; // Arguments to the ALU

	mem_access_type_t mem_access_type; // Setup read/write from memory
	uint8_t           mem_bytes;       // Number of bytes to read/write (0, 1, 2 or 4)
	uint32_t          mem_write_value;

	// The address of the register to write back to
	// If this is 0, no writeback occurs
	uint8_t writeback_reg;
} decode_bundle_t;

decode_bundle_t decode_instruction(const mips_state_t *state);
mips_instr_t    parse_instruction(const mips_state_t *state);

#endif
