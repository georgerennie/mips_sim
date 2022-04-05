UNIT := $(TEST)/unit

UNIT_SRCS += $(UTIL_SRCS)
UNIT_SRCS += $(CORE_SRCS)
UNIT_SRCS += $(REF_CORE_SRCS)
UNIT_SRCS += $(wildcard $(UNIT)/*.c)

UNIT_BIN := $(BUILD)/unit_tests
UNIT_BUILD := $(BUILD)/unit
UNIT_FLAGS += -DUNIT_TEST

.PHONY: unit
unit: $(UNIT_BIN)
	@./$(UNIT_BIN)

-include $(UNIT_SRCS:%=$(UNIT_BUILD)/%.d)
$(UNIT_BIN): $(UNIT_SRCS:%=$(UNIT_BUILD)/%.o)
	$(call setup_target,$@)
	@$(CXX) $(CXX_FLAGS) $(UNIT_FLAGS) $^ -o $@

$(UNIT_BUILD)/%.c.o: %.c
	$(call setup_target,$@)
	@$(CC) $(CC_FLAGS) $(UNIT_FLAGS) -c $< -o $@

$(UNIT_BUILD)/%.cpp.o: %.cpp
	$(call setup_target,$@)
	@$(CXX) $(CXX_FLAGS) $(UNIT_FLAGS) -c $< -o $@
