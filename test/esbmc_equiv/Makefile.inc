ESBMC_EQUIV := $(TEST)/esbmc_equiv
ESBMC_EQUIV_BUILD := $(BUILD)/esbmc_equiv
ESBMC_EQUIV_MAIN := $(ESBMC_EQUIV_BUILD)/main.c

ESBMC_EQUIV_INSTRS := 2
ESBMC_EQUIV_CYCLES := 7
ESBMC_EQUIV_REF_CYCLES := 3
ESBMC_EQUIV_GRAPHML := $(ESBMC_EQUIV_BUILD)/trace.graphml
ESBMC_EQUIV_TRACE := $(UNIT)/esbmc_equiv_trace.c

ESBMC_EQUIV_PY := $(ESBMC_EQUIV)/esbmc_equiv.py

ESBMC_EQUIV_FLAGS += $(ESBMC_FLAGS)
ESBMC_EQUIV_FLAGS += --memory-leak-check --overflow-check --struct-fields-check
ESBMC_EQUIV_FLAGS += --initialize-nondet-variables
ESBMC_EQUIV_FLAGS += --interval-analysis --add-symex-value-sets
ESBMC_EQUIV_FLAGS += --max-k-step 64 --k-step 32
ESBMC_EQUIV_FLAGS += --incremental-bmc --unroll-loops
ESBMC_EQUIV_FLAGS += --witness-output $(ESBMC_EQUIV_GRAPHML)
ESBMC_EQUIV_FLAGS += -DESBMC_MAX_MEM=8

ESBMC_EQUIV_SRCS += $(EQUIV_CHECK_SRCS)

.PHONY: esbmc_equiv
esbmc_equiv: $(ESBMC_EQUIV_MAIN) $(ESBMC_EQUIV_SRCS)
	#@mv $(ESBMC_EQUIV_GRAPHML) $(ESBMC_EQUIV_GRAPHML).bak
	#@$(ESBMC) $^ $(ESBMC_EQUIV_FLAGS) ||
		python3 $(ESBMC_EQUIV_PY) -c $(ESBMC_EQUIV_CYCLES) \
			-r $(ESBMC_EQUIV_REF_CYCLES) -g $(ESBMC_EQUIV_GRAPHML) \
			--add-id $(ESBMC_EQUIV_TRACE)

$(ESBMC_EQUIV_MAIN): $(ESBMC_EQUIV_PY) $(ESBMC_EQUIV)/template.c.jinja2 $(ESBMC_EQUIV)/Makefile.inc
	$(call setup_target,$@)
	python3 $< -c $(ESBMC_EQUIV_CYCLES) -r $(ESBMC_EQUIV_REF_CYCLES) -n $(ESBMC_EQUIV_INSTRS) $@
