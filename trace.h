#ifndef _TRACE_H
#define _TRACE_H
#include "machine.h"

// Prints state of registers and memory
extern void trace_state(reg_type GPR[], mem_u memory, address_type PC, reg_type HI, reg_type LO);

// Prints address and assembly form of instruction
extern void trace_instr(address_type PC, bin_instr_t bi);

#endif
