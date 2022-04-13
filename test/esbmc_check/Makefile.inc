ESBMC_CHECK_FLAGS += $(ESBMC_FLAGS)
ESBMC_CHECK_FLAGS += --memory-leak-check --overflow-check --struct-fields-check
ESBMC_CHECK_FLAGS += --interval-analysis --add-symex-value-sets
ESBMC_CHECK_FLAGS += --max-k-step 64 --k-step 40
ESBMC_CHECK_FLAGS += --incremental-bmc

.PHONY: esbmc_check
esbmc_check: $(TEST)/esbmc_check/main.c $(CORE_SRCS)
	@$(ESBMC) $^ $(ESBMC_CHECK_FLAGS)
