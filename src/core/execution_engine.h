#ifndef _EXECUTION_ENGINE_H_
#define _EXECUTION_ENGINE_H_

#include <inttypes.h>
#include "instruction_decoder.h"

typedef struct {
	uint32_t result;

	// Passthrough from ID
	mem_access_type_t mem_access_type;
	uint8_t           mem_bytes;
	uint8_t           writeback_reg;
} execution_bundle_t;

execution_bundle_t execute_instruction(const decode_bundle_t *bundle);

#endif
