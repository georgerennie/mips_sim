include $(SRC)/common/Makefile.inc
include $(SRC)/core/Makefile.inc
include $(SRC)/ref_core/Makefile.inc
include $(SRC)/equiv_check/Makefile.inc

CLI_SRCS += $(EQUIV_CHECK_SRCS)
CLI_SRCS += $(SRC)/assembler.cpp
CLI_SRCS += $(SRC)/sim_runner.cpp
CLI_SRCS += $(SRC)/main.cpp

CLI_BIN := $(BUILD)/$(TARGET)
CLI_BUILD := $(BUILD)/cli

.PHONY: build_cli
build_cli: $(CLI_BIN)

-include $(CLI_SRCS:%=$(CLI_BUILD)/%.d)
$(CLI_BIN): $(CLI_SRCS:%=$(CLI_BUILD)/%.o)
	$(call setup_target,$@)
	@$(CXX) $(CXX_FLAGS) $^ -o $@

$(CLI_BUILD)/%.c.o: %.c
	$(call setup_target,$@)
	@$(CC) $(CC_FLAGS) -c $< -o $@

$(CLI_BUILD)/%.cpp.o: %.cpp
	$(call setup_target,$@)
	@$(CXX) $(CXX_FLAGS) -c $< -o $@
