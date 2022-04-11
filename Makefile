############## Settings ########################################################
TARGET := mips_sim
CC := clang
CXX := clang++
ESBMC := esbmc

SRC := src
TEST := test
BUILD := build

CC_INCLUDES += -I$(SRC) -I$(TEST)

CC_WARNINGS += -Wall -Wpedantic -Wextra -Wconversion -Wshadow -Wdouble-promotion
CC_WARNINGS += -Wunused-parameter -Wsign-conversion -Wfloat-conversion
CC_WARNINGS += -Wconstant-conversion -Wint-conversion -Wimplicit-fallthrough
CC_WARNINGS += -Wundef -Wunused-variable # -Wpadded

ifdef DEBUG_PRINT
	CC_CXX_FLAGS += -DDEBUG_PRINT=$(DEBUG_PRINT)
endif

CC_CXX_FLAGS += $(CC_WARNINGS) $(CC_INCLUDES) -O3 -flto -MMD
CC_FLAGS += $(CC_CXX_FLAGS) -std=c11
CXX_FLAGS += $(CC_CXX_FLAGS) -std=c++20

ESBMC_FLAGS += $(CC_INCLUDES) $(CC_WARNINGS) -Wno-newline-eof -Wno-unused-parameter
ESBMC_FLAGS += -Wno-implicit-fallthrough
ESBMC_FLAGS += -DESBMC --memlimit 10g

############## Pretty Printing #################################################
GREEN_ANSI := \033[0;32m
NC_ANSI := \033[0m

define setup_target
	@echo -e "--- $(GREEN_ANSI)Building $(1)$(NC_ANSI)"
	@mkdir -p $(dir $(1))
endef

############## Compile Rules ###################################################
# Include source/cli rules
include $(SRC)/Makefile.inc
# Include test config rules
include $(TEST)/Makefile.inc

.PHONY: build
build: build_cli

.PHONY: clean
clean:
	rm -rf $(BUILD)
