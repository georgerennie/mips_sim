#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

typedef enum {
	MIPS_OPC_R_FMT = 0x00,

	MIPS_OPC_ADD   = 0x00,
	MIPS_OPC_ADDU  = 0x00,
	MIPS_OPC_ADDI  = 0x08,
	MIPS_OPC_ADDIU = 0x09,

	MIPS_OPC_SUB  = 0x00,
	MIPS_OPC_SUBU = 0x00,

	MIPS_OPC_AND  = 0x00,
	MIPS_OPC_OR   = 0x00,
	MIPS_OPC_XOR  = 0x00,
	MIPS_OPC_NOR  = 0x00,
	MIPS_OPC_ANDI = 0x0C,
	MIPS_OPC_ORI  = 0x0D,

	MIPS_OPC_LUI = 0x0F,

	// MIPS_OPC_SB  = 0x28,
	// MIPS_OPC_SH  = 0x29,
	// MIPS_OPC_SW  = 0x2B,
	MIPS_OPC_LB  = 0x20,
	MIPS_OPC_LBU = 0x24,
	MIPS_OPC_LHU = 0x25,
	MIPS_OPC_LW  = 0x23,

	MIPS_OPC_J = 0x02,
} mips_opcode_t;

typedef enum {
	MIPS_FUNCT_NONE = 0x00,

	MIPS_FUNCT_ADD  = 0x20,
	MIPS_FUNCT_ADDU = 0x21,
	MIPS_FUNCT_SUB  = 0x22,
	MIPS_FUNCT_SUBU = 0x23,

	MIPS_FUNCT_AND = 0x24,
	MIPS_FUNCT_OR  = 0x25,
	MIPS_FUNCT_XOR = 0x26,
	MIPS_FUNCT_NOR = 0x27,
} mips_funct_t;

typedef enum {
	MIPS_INSTR_FORMAT_UNKNOWN = 0,
	MIPS_INSTR_FORMAT_R,
	MIPS_INSTR_FORMAT_I,
	MIPS_INSTR_FORMAT_J,
} mips_instr_format_t;

typedef struct {
	uint8_t rs;
	uint8_t rt;
	uint8_t rd;
	uint8_t shamt;
	uint8_t funct;
} mips_r_instr_data_t;

typedef struct {
	uint8_t  rs;
	uint8_t  rt;
	uint16_t immediate;
} mips_i_instr_data_t;

typedef struct {
	uint32_t address;
} mips_j_instr_data_t;

typedef struct {
	mips_opcode_t       opcode;
	mips_instr_format_t format;

	union {
		mips_r_instr_data_t r_data;
		mips_i_instr_data_t i_data;
		mips_j_instr_data_t j_data;
	};

} mips_instr_t;

static const char* mips_reg_lookup[32] = {"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
                                          "t0",   "t1", "t2", "t3", "t4", "t5", "t6", "t7",
                                          "s0",   "s1", "s2", "s3", "s4", "s4", "s6", "s7",
                                          "t8",   "t9", "k0", "k1", "gp", "sp", "fp", "ra"};

#ifdef __cplusplus
}
#endif

#endif
