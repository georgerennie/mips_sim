#ifndef INSTRUCTION_DECODE_H
#define INSTRUCTION_DECODE_H

#include <inttypes.h>
#include "arch_state.h"
#include "execute.h"

id_ex_reg_t instruction_decode(uint32_t instr, const mips_state_t* arch_state);

#endif
