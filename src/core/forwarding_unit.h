#ifndef FORWARDING_UNIT_H
#define FORWARDING_UNIT_H

#include "core.h"

uint32_t get_fwd_value(const mips_pipeline_regs_t* regs, uint8_t reg, uint32_t fallback);

#endif
