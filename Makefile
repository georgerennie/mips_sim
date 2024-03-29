############## Settings ########################################################
TARGET := mips_sim
CC := clang
CXX := clang++
ESBMC ?= esbmc

SRC := src
TEST := test
SHOWCASE := showcase_program
BUILD := build

CC_INCLUDES += -I$(SRC) -I$(TEST)

CC_WARNINGS += -Wall -Wpedantic -Wextra -Wconversion -Wshadow -Wdouble-promotion
CC_WARNINGS += -Wunused-parameter -Wsign-conversion -Wfloat-conversion
CC_WARNINGS += -Wconstant-conversion -Wint-conversion -Wimplicit-fallthrough
CC_WARNINGS += -Wundef -Wunused-variable # -Wpadded

ifdef DEBUG_PRINT
	CC_CXX_FLAGS += -DDEBUG_PRINT=$(DEBUG_PRINT)
endif

CC_CXX_FLAGS += $(CC_WARNINGS) $(CC_INCLUDES) -MMD
CC_FLAGS += $(CC_CXX_FLAGS) -std=c11
CXX_FLAGS += $(CC_CXX_FLAGS) -std=c++20

ESBMC_FLAGS += $(CC_INCLUDES) $(CC_WARNINGS) -Wno-newline-eof -Wno-unused-parameter
ESBMC_FLAGS += -Wno-implicit-fallthrough
ESBMC_FLAGS += -DESBMC --memlimit 10g

############## Pretty Printing #################################################
GREEN_ANSI := \033[32m
MAG_ANSI   := \033[35m
NC_ANSI    := \033[0m

define setup_target
	@echo -e "--- $(GREEN_ANSI)Building $(1)$(NC_ANSI)"
	@mkdir -p $(dir $(1))
endef

############## Compile Rules ###################################################
# Include source/cli rules
include $(SRC)/Makefile.inc
# Include test config rules
include $(TEST)/Makefile.inc
# Include showcase program rules
include $(SHOWCASE)/Makefile.inc

.PHONY: build short_tests
build: build_cli
short_tests: unit esbmc_unit asm_test

.PHONY: clean
clean:
	rm -rf $(BUILD)
