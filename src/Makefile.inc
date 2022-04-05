include $(SRC)/util/Makefile.inc
include $(SRC)/core/Makefile.inc
include $(SRC)/ref_core/Makefile.inc

CLI_SRCS += $(UTIL_SRCS)
CLI_SRCS += $(CORE_SRCS)
CLI_SRCS += $(SRC)/assembler/assembler.cpp
CLI_SRCS += $(SRC)/main.cpp

CLI_BIN := $(BUILD)/$(TARGET)

.PHONY: build_cli
build_cli: $(CLI_BIN)

-include $(CLI_SRCS:%=$(BUILD)/%.d)
$(CLI_BIN): $(CLI_SRCS:%=$(BUILD)/%.o)
	$(call setup_target,$@)
	@$(CXX) $(CXX_FLAGS) $^ -o $@

