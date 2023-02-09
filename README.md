# MIPS Simulator

This repo contains a cycle accurate emulator for a 5-stage in-order MIPS
processor (Patterson and Hennessy style) supporting a subset of instructions.
This was developed for a university coursework. I then extended the coursework
by applying formal verification using ESBMC to verify the core against a simple
single cycle reference core. Due to the lack of supported instructions, I dont
advise using this emulator, but it may be interesting to look at the methodology
employed. A more complete writeup is available in [my report](report.pdf)

The emulator can be built with `make`.

Some notable paths:
- `src/core`: The main verified emulator core written in C11
- `src/ref_core`: The reference core that the pipelined emulator was verified against
- `src/equiv_check`: The assertions used for equivalence checking the cores
- `test/esbmc_isa`: ISA-Formal inspired equivalence checking using ESBMC

There are also various other integration, unit and formal tests in subfolders of
`test`. They can generally be run with `make <folder_name>`.
