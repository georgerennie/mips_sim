SHOWCASE_SRC := $(SHOWCASE)/square_to_200.delay.asm
SHOWCASE_NO_DLY_SRC := $(SHOWCASE)/square_to_200.asm

.PHONY: showcase showcase_step showcase_no_delay showcase_no_delay_step

showcase: $(SHOWCASE_SRC) $(CLI_BIN)
	$(CLI_BIN) $< -cdm 512

showcase_step: $(SHOWCASE_SRC) $(CLI_BIN)
	$(CLI_BIN) $< -cdsm 512

showcase_no_delay: $(SHOWCASE_NO_DLY_SRC) $(CLI_BIN)
	$(CLI_BIN) $< -cm 512

showcase_no_delay_step: $(SHOWCASE_NO_DLY_SRC) $(CLI_BIN)
	$(CLI_BIN) $< -csm 512
