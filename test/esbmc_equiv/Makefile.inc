ESBMC_EQUIV_FLAGS += $(ESBMC_FLAGS)
ESBMC_EQUIV_FLAGS += --memory-leak-check --overflow-check --struct-fields-check
ESBMC_EQUIV_FLAGS += --interval-analysis --add-symex-value-sets
ESBMC_EQUIV_FLAGS += --max-k-step 32 --k-step 8
ESBMC_EQUIV_FLAGS += --incremental-bmc
# ESBMC_EQUIV_FLAGS += --witness-output out.graphml

.PHONY: esbmc_equiv
esbmc_equiv: $(TEST)/esbmc_equiv/main.c $(UTIL_SRCS) $(CORE_SRCS) $(REF_CORE_SRCS)
	@$(ESBMC) $^ $(ESBMC_EQUIV_FLAGS)
