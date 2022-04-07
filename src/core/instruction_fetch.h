#ifndef INSTRUCTION_FETCH_H
#define INSTRUCTION_FETCH_H

#include <inttypes.h>
#include "arch_state.h"
#include "util/util.h"

uint32_t instruction_fetch(const mips_state_t* arch_state, span_t instr_mem);

#endif
