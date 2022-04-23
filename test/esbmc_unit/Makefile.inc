ESBMC_UNIT_FLAGS += $(ESBMC_FLAGS)
ESBMC_UNIT_FLAGS += --memory-leak-check --overflow-check --struct-fields-check
ESBMC_UNIT_FLAGS += --interval-analysis --add-symex-value-sets
ESBMC_UNIT_FLAGS += --incremental-bmc

.PHONY: esbmc_unit
esbmc_unit: $(TEST)/esbmc_unit/main.c $(TEST_COMMON_SRCS) $(CORE_SRCS)
	@$(ESBMC) $^ $(ESBMC_UNIT_FLAGS)
