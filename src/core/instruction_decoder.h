#ifndef _INSTRUCTION_DECODER_H_
#define _INSTRUCTION_DECODER_H_

#include <inttypes.h>
#include "arch_state.h"
#include "execution_engine.h"
#include "instruction.h"

execute_bundle_t decode_instruction(const mips_state_t* state, uint32_t instruction);
mips_instr_t     parse_instruction(uint32_t instr);

#endif
