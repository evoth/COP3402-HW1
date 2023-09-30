#ifndef _MACHINE_H
#define _MACHINE_H
#include "instruction.h"
#include "machine_types.h"
#include "regname.h"

#define MEMORY_SIZE_IN_BYTES (65536 - BYTES_PER_WORD)
#define MEMORY_SIZE_IN_WORDS (MEMORY_SIZE_IN_BYTES / BYTES_PER_WORD)

// Memory union type
typedef union
{
    byte_type bytes[MEMORY_SIZE_IN_BYTES];
    bin_instr_t instrs[MEMORY_SIZE_IN_WORDS];
    word_type words[MEMORY_SIZE_IN_WORDS];
} mem_u;

// Register type
typedef int reg_type;

// Initialize machine state from binary object file
extern void machine_init(const char *filename);

// Executes loaded program and prints tracing ouput
extern void machine_exec();

// Prints assembly instruction listing and initial data values
extern void machine_print_program();

#endif
