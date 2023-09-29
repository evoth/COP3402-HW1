#ifndef _MACHINE_H
#define _MACHINE_H
#include "instruction.h"
#include "machine_types.h"
#include "regname.h"

#define MEMORY_SIZE_IN_BYTES (65536 - BYTES_PER_WORD)

// Memory union type
typedef union
{
    byte_type bytes[MEMORY_SIZE_IN_BYTES];
    bin_instr_t instrs[MEMORY_SIZE_IN_BYTES];
    word_type words[MEMORY_SIZE_IN_BYTES];
} mem_u;

// Initialize machine state from binary object file
extern void machine_init(const char *filename);

#endif
