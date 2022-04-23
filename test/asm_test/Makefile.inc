ASM_TEST_SRCS += $(wildcard $(TEST)/asm_test/*.asm)
ASM_TEST_TARGETS := $(ASM_TEST_SRCS:.asm=.asm.test)

ASM_TEST_FLAGS := --compare --quiet --mem-size 1024

.PHONY: asm_test $(ASM_TEST_TARGETS)
asm_test: $(ASM_TEST_TARGETS)

$(ASM_TEST_TARGETS): %.asm.test: %.asm $(CLI_BIN)
	@echo -e "- $(MAG_ANSI)Running$(NC_ANSI) $<"
	@$(CLI_BIN) $(ASM_TEST_FLAGS) $<

