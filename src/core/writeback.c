#include "writeback.h"

void writeback(mips_state_t* state, const writeback_bundle_t* bundle) {
	gpr_write(state, bundle->reg, bundle->value);
}

inline void writeback_bundle_init(writeback_bundle_t* bundle) {
	bundle->reg   = 0;
	bundle->value = 0;
}
