#ifndef CORE_H
#define CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "core_state.h"

void mips_core_init(mips_core_t* core, span_t instr_mem, span_t data_mem);

// Execute instructions until one generates a trap
mips_trap_t mips_core_run(mips_core_t* core);

// Execute one clock cycle
mips_trap_t mips_core_cycle(mips_core_t* core);

#ifdef __cplusplus
}
#endif

#endif
