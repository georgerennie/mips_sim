#ifndef FORWARDING_UNIT_H
#define FORWARDING_UNIT_H

#include "common/arch.h"
#include "core.h"

uint32_t get_fwd_value(const mips_pipeline_regs_t* regs, mips_reg_idx_t reg, uint32_t fetched_val);

#endif
