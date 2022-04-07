#ifndef INSTRUCTION_DECODE_H
#define INSTRUCTION_DECODE_H

#include <inttypes.h>
#include "arch_state.h"
#include "execute.h"

typedef struct {
	// Data
	uint32_t instruction;
	uint32_t address;
} if_id_reg_t;

id_ex_reg_t instruction_decode(if_id_reg_t* if_id, const mips_state_t* arch_state);
void        if_id_reg_init(if_id_reg_t* if_id);

#endif
