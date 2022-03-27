############## Settings ########################################################
TARGET ?= mips_sim
CC ?= clang
ESBMC ?= esbmc
BUILD ?= build

SRC := src
SRCS := \
	$(SRC)/main.c \
	$(SRC)/util.c \
	$(SRC)/asm_frontend.c

CC_WARNINGS += -Wall -Wpedantic -Wextra -Wconversion -Wshadow -Wdouble-promotion
CC_WARNINGS += -Wunused-parameter -Wpadded
CC_WARNINGS += -Wsign-conversion -Wfloat-conversion -Wundef -Wstack-usage=16
CC_WARNINGS += -Wformat=2 -Wformat-overflow -Wformat-truncation

CC_FLAGS += $(CC_WARNINGS) -O3 -flto -std=c11 -MMD
ESBMC_FLAGS += $(CC_WARNINGS) -D__ESBMC

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
check:
	$(ESBMC) $(SRCS) --memory-leak-check --overflow-check --struct-fields-check
