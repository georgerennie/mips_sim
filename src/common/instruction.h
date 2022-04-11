#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "util.h"

typedef enum {
	MIPS_OPC_R_FMT = 0x00,

	MIPS_OPC_ADDU  = 0x00,
	MIPS_OPC_ADDIU = 0x09,

	MIPS_OPC_AND  = 0x00,
	MIPS_OPC_OR   = 0x00,
	MIPS_OPC_ANDI = 0x0C,
	MIPS_OPC_ORI  = 0x0D,

	MIPS_OPC_BEQ = 0x04,
	MIPS_OPC_BNE = 0x05,

	MIPS_OPC_SH  = 0x29,
	MIPS_OPC_LHU = 0x25,

	MIPS_OPC_J = 0x02,
} ATTR_PACKED mips_opcode_t;

typedef enum {
	// This is actually variant of sll, but we are treating it like a nop no
	// matter what arguments are provided
	MIPS_FUNCT_NOP = 0x00,

	MIPS_FUNCT_ADDU = 0x21,
	MIPS_FUNCT_AND  = 0x24,
	MIPS_FUNCT_OR   = 0x25,
} ATTR_PACKED mips_funct_t;

typedef enum {
	MIPS_INSTR_FORMAT_UNKNOWN = 0,
	MIPS_INSTR_FORMAT_R,
	MIPS_INSTR_FORMAT_I,
	MIPS_INSTR_FORMAT_J,
} ATTR_PACKED mips_instr_format_t;

typedef struct {
	uint8_t rs;
	uint8_t rt;
	uint8_t rd;
	uint8_t shamt;
	uint8_t funct;
} ATTR_PACKED mips_r_instr_data_t;

typedef struct {
	uint8_t  rs;
	uint8_t  rt;
	uint16_t immediate;
} ATTR_PACKED mips_i_instr_data_t;

typedef struct {
	uint32_t address;
} ATTR_PACKED mips_j_instr_data_t;

typedef struct {
	mips_opcode_t       opcode;
	mips_instr_format_t format;

	union {
		mips_r_instr_data_t r_data;
		mips_i_instr_data_t i_data;
		mips_j_instr_data_t j_data;
	};

} ATTR_PACKED mips_instr_t;

const char* mips_instr_name(uint32_t instruction);

extern const char* mips_reg_lookup[32];

#ifdef __cplusplus
}
#endif

#endif
