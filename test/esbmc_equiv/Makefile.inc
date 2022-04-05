ESBMC_EQUIV := $(TEST)/esbmc_equiv
ESBMC_EQUIV_BUILD := $(BUILD)/$(ESBMC_EQUIV)
ESBMC_EQUIV_MAIN := $(ESBMC_EQUIV_BUILD)/main.c

ESBMC_EQUIV_INSTRS := 2
ESBMC_EQUIV_CYCLES := 7
ESBMC_EQUIV_REF_CYCLES := 3
ESBMC_EQUIV_GRAPHML := $(ESBMC_EQUIV_BUILD)/trace.graphml
ESBMC_EQUIV_TRACE := $(ESBMC_EQUIV)/trace.c

ESBMC_EQUIV_PY := $(ESBMC_EQUIV)/esbmc_equiv.py

ESBMC_EQUIV_FLAGS += $(ESBMC_FLAGS)
ESBMC_EQUIV_FLAGS += --memory-leak-check --overflow-check --struct-fields-check
ESBMC_EQUIV_FLAGS += --interval-analysis --add-symex-value-sets
ESBMC_EQUIV_FLAGS += --max-k-step 32 --k-step 10
ESBMC_EQUIV_FLAGS += --incremental-bmc
ESBMC_EQUIV_FLAGS += --witness-output $(ESBMC_EQUIV_GRAPHML)

ESBMC_EQUIV_SRCS += $(UTIL_SRCS) $(CORE_SRCS) $(REF_CORE_SRCS)

.PHONY: esbmc_equiv
esbmc_equiv: $(ESBMC_EQUIV_MAIN) $(ESBMC_EQUIV_SRCS)
	@$(ESBMC) $^ $(ESBMC_EQUIV_FLAGS) || \
		python3 $(ESBMC_EQUIV_PY) -c $(ESBMC_EQUIV_CYCLES) \
			-r $(ESBMC_EQUIV_REF_CYCLES) -g $(ESBMC_EQUIV_GRAPHML) \
			$(ESBMC_EQUIV)/trace.c

$(ESBMC_EQUIV_MAIN): $(ESBMC_EQUIV_PY) $(ESBMC_EQUIV)/template.c.jinja2 $(ESBMC_EQUIV)/Makefile.inc
	$(call setup_target,$@)
	python3 $< -c $(ESBMC_EQUIV_CYCLES) -r $(ESBMC_EQUIV_REF_CYCLES) -n $(ESBMC_EQUIV_INSTRS) $@

ESBMC_EQUIV_BIN := $(BUILD)/esbmc_equiv_trace

.PHONY: esbmc_equiv_trace
esbmc_equiv_trace: $(ESBMC_EQUIV_BIN)

ESBMC_EQUIV_SRCS += $(ESBMC_EQUIV_TRACE)
-include $(ESBMC_EQUIV_SRCS:%=$(BUILD)/%.d)
$(ESBMC_EQUIV_BIN): $(ESBMC_EQUIV_SRCS:%=$(BUILD)/%.o)
	$(call setup_target,$@)
	@$(CXX) $(CXX_FLAGS) $^ -o $@
