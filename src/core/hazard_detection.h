#ifndef HAZARD_DETECTION_H
#define HAZARD_DETECTION_H

#include "core.h"

typedef struct {
	// If a stage is flushed, it is converted to a nop
	bool flushes[MIPS_STAGE_NUM - 2];

	// If a stage is stalled, it doesnt write out its value
	bool stalls[MIPS_STAGE_NUM - 1];
} hazard_flags_t;

hazard_flags_t detect_hazards(
    const mips_pipeline_regs_t* regs, const mips_pipeline_regs_t* next_regs,
    const mips_config_t* config);

#endif
