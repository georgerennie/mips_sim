SHOWCASE_SRC := $(SHOWCASE)/showcase.delay.asm
SHOWCASE_NO_DLY_SRC := $(SHOWCASE)/showcase.asm
SHOWCASE_UNOPT_SRC := $(SHOWCASE)/unoptimised.asm

.PHONY: showcase showcase_step showcase_no_delay showcase_no_delay_step showcase_unoptimised showcase_unoptimised_step

showcase: $(SHOWCASE_SRC) $(CLI_BIN)
	$(CLI_BIN) $< -cdm 512

showcase_step: $(SHOWCASE_SRC) $(CLI_BIN)
	$(CLI_BIN) $< -cdsm 512

showcase_no_delay: $(SHOWCASE_NO_DLY_SRC) $(CLI_BIN)
	$(CLI_BIN) $< -cm 512

showcase_no_delay_step: $(SHOWCASE_NO_DLY_SRC) $(CLI_BIN)
	$(CLI_BIN) $< -csm 512

showcase_unoptimised: $(SHOWCASE_UNOPT_SRC) $(CLI_BIN)
	$(CLI_BIN) $< -cm 1024

showcase_unoptimised_step: $(SHOWCASE_UNOPT_SRC) $(CLI_BIN)
	$(CLI_BIN) $< -csm 1024
