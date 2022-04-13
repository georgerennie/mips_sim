#ifndef EQUIV_CHECK_H
#define EQUIV_CHECK_H

#include "common/arch.h"
#include "core/core.h"
#include "ref_core/ref_core.h"

void equiv_check_gprs(const mips_core_t* sim_core, const mips_ref_core_t* ref_core);

void equiv_check_mems(const span_t sim_mem, const span_t ref_mem);
void equiv_check_instr_mems(const mips_core_t* sim_core, const mips_ref_core_t* ref_core);
void equiv_check_data_mems(const mips_core_t* sim_core, const mips_ref_core_t* ref_core);

void equiv_check_configs(const mips_core_t* sim_core, const mips_ref_core_t* ref_core);
void equiv_check_cores(const mips_core_t* sim_core, const mips_ref_core_t* ref_core);

void equiv_check_exceptions(
    const mips_exception_t* sim_exception, const mips_exception_t* ref_exception);
void equiv_check_retires(
    const mips_retire_metadata_t* sim_retire, const mips_retire_metadata_t* ref_retire);

#endif
