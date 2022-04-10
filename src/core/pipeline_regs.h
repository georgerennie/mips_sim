#ifndef PIPELINE_REGS_H
#define PIPELINE_REGS_H

#include <inttypes.h>
#include <stdbool.h>
#include "util/arch_structs.h"
#include "util/util.h"

// -------- MEM/WB -------------------------------------------------------------

typedef struct {
	// Data
	uint8_t  reg; // Register to write to. No write if reg == 0
	uint32_t result;

	// Metadata
	mips_retire_metadata_t metadata;
} mem_wb_reg_t;

// -------- EX/MEM -------------------------------------------------------------

typedef enum {
	MEM_ACCESS_NONE = 0,
	MEM_ACCESS_READ_UNSIGNED,
	MEM_ACCESS_READ_SIGNED, // Currently unused
	MEM_ACCESS_WRITE,
} ATTR_PACKED memory_access_t;

typedef struct {
	// Control
	memory_access_t access_type;
	uint8_t         bytes; // Number of bytes to R/W. No R/W if 0

	// Data
	uint32_t data_rt; // RT value for writeback

	// Data / Pipeline passthrough
	// Destination reg is set to 0 if memory store occurs
	mem_wb_reg_t mem_wb;
} ex_mem_reg_t;

// -------- ID/EX --------------------------------------------------------------

typedef enum {
	ALU_OP_NOP = 0,

	ALU_OP_ADD,
	ALU_OP_AND,
	ALU_OP_OR,
} ATTR_PACKED alu_op_t;

typedef enum {
	ALU_SRC_DATA_B = 0, // Use value read from 2nd register in ID
	ALU_SRC_IMM,        // Use immediate generated in ID
} ATTR_PACKED alu_src_t;

typedef struct {
	// Data
	uint32_t data_rs, data_rt; // Arguments to the ALU
	uint8_t  reg_rs, reg_rt;   // Registers of data_rs and data_rt for forwarding
	uint32_t immediate;

	// Branch data is read by hazard detection, not execute
	bool eval_branch; // Does this instruction evaluate a branch, i.e. need stalling for branch args
	bool branch;      // Does this instruction branch
	uint32_t branch_address; // Branch/jump address calculated in ID

	// Control
	alu_op_t  alu_op;    // ALU Operation
	alu_src_t alu_b_src; // Select data_b/forwarded value or immediate as b input

	// Pipeline passthrough
	ex_mem_reg_t ex_mem;
} id_ex_reg_t;

// -------- IF/ID --------------------------------------------------------------

typedef struct {
	// Data
	uint32_t instruction;
	uint32_t address;

	// Metadata
	mips_retire_metadata_t metadata;
} if_id_reg_t;

// -------- Whole Pipeline -----------------------------------------------------

typedef enum {
	MIPS_STAGE_NONE = -1,

	MIPS_STAGE_IF = 0,
	MIPS_STAGE_ID,
	MIPS_STAGE_EX,
	MIPS_STAGE_MEM,
	MIPS_STAGE_WB,

	MIPS_STAGE_NUM,
} ATTR_PACKED mips_core_stage_t;

typedef struct {
	if_id_reg_t  if_id;
	id_ex_reg_t  id_ex;
	ex_mem_reg_t ex_mem;
	mem_wb_reg_t mem_wb;
} mips_pipeline_regs_t;

// -------- Function -----------------------------------------------------------

void log_pipeline_regs(const mips_pipeline_regs_t* regs);

const char* mem_access_to_str(memory_access_t access);
const char* alu_op_to_str(alu_op_t op);
const char* alu_src_to_str(alu_src_t src);

void if_id_reg_init(if_id_reg_t* if_id);
void id_ex_reg_init(id_ex_reg_t* id_ex);
void ex_mem_reg_init(ex_mem_reg_t* ex_mem);
void mem_wb_reg_init(mem_wb_reg_t* mem_wb);
void pipeline_regs_init(mips_pipeline_regs_t* regs);

#endif
