ESBMC_ISA_GRAPHML := $(BUILD)/esbmc_isa/trace.graphml

ESBMC_ISA_FLAGS += $(ESBMC_FLAGS)
# ESBMC_ISA_FLAGS += --no-bounds-check --no-div-by-zero-check --no-pointer-check
# ESBMC_ISA_FLAGS += --no-align-check --no-pointer-relation-check
ESBMC_ISA_FLAGS += --interval-analysis --add-symex-value-sets
ESBMC_ISA_FLAGS += --max-k-step 64 --k-step 40
ESBMC_ISA_FLAGS += --incremental-bmc
ESBMC_ISA_FLAGS += --big-endian
ESBMC_ISA_FLAGS += --witness-output $(ESBMC_ISA_GRAPHML)

.PHONY: esbmc_isa
esbmc_isa: $(TEST)/esbmc_isa/main.c $(EQUIV_CHECK_SRCS)
	@mkdir -p $(BUILD)/esbmc_isa/
	@$(ESBMC) $^ $(ESBMC_ISA_FLAGS)
