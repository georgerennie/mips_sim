############## Settings ########################################################
TARGET ?= mips_sim
CC ?= clang
ESBMC ?= esbmc
BUILD ?= build

SRC := src
SRCS := \
	$(SRC)/main.c \
	$(SRC)/util.c \
	\
	$(SRC)/core/core.c \
	$(SRC)/core/arch_state.c \
	$(SRC)/core/instruction_decoder.c \
	$(SRC)/core/execution_engine.c \
	$(SRC)/core/memory_access.c \
	$(SRC)/core/writeback.c

CC_INCLUDES += -I$(SRC)

CC_WARNINGS += -Wall -Wpedantic -Wextra -Wconversion -Wshadow -Wdouble-promotion
CC_WARNINGS += -Wunused-parameter -Wsign-conversion -Wfloat-conversion
CC_WARNINGS += -Wundef # -Wpadded

CC_FLAGS += $(CC_WARNINGS) $(CC_INCLUDES) -O3 -flto -std=c11 -MMD

ESBMC_FLAGS += $(CC_INCLUDES) $(CC_WARNINGS) -Wno-newline-eof -Wno-unused-parameter
ESBMC_FLAGS += -D__ESBMC --memlimit 8g

ESBMC_CHECK_FLAGS += --memory-leak-check --overflow-check --struct-fields-check
ESBMC_CHECK_FLAGS += --interval-analysis
ESBMC_CHECK_FLAGS += --k-induction-parallel --k-step 10

############## Auto Config #####################################################
OBJS := $(SRCS:$(SRC)%.c=$(BUILD)%.o)
DEPS = $(OBJS:%.o=%.d)

MAKEFILE := Makefile
TARGET_BIN := $(BUILD)/$(TARGET)

GREEN_ANSI := \033[0;32m
NC_ANSI := \033[0m

define setup_target
	@echo -e "--- $(GREEN_ANSI)Building $(1)$(NC_ANSI)"
	@mkdir -p $(dir $(1))
endef

############## Compile Rules ###################################################
.PHONY: build
build: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(call setup_target,$@)
	@$(CC) $(CC_FLAGS) $^ -o $@

-include $(DEPS)
$(BUILD)/%.o: $(SRC)/%.c $(MAKEFILE)
	$(call setup_target,$@)
	@$(CC) $(CC_FLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD)

############## ESBMC Rules #####################################################

.PHONY: check

# General check of useful single threaded properties, as well as embedded
# properties with esbmc. Runs on the whole program
check: $(SRCS)
	@$(ESBMC) $^ $(ESBMC_FLAGS) $(ESBMC_CHECK_FLAGS)
