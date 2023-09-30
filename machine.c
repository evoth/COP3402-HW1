#include <stdio.h>
#include <string.h>
#include "machine.h"
#include "trace.h"

// General purpose registers
reg_type GPR[NUM_REGISTERS];

// Program memory
mem_u memory;

// Special purpose registers
address_type PC;
reg_type HI, LO;

// Binary file
BOFFILE bf;

// Binary header object
BOFHeader bh;

// Initialize machine state from binary object file
void machine_init(const char *filename)
{
    // Open binary object file and read header
    bf = bof_read_open(filename);
    bh = bof_read_header(bf);

    // Initialize memory and registers to 0
    memset(&memory, 0, bh.stack_bottom_addr + BYTES_PER_WORD);
    memset(GPR, 0, sizeof(reg_type) * NUM_REGISTERS);

    // Load instructions
    const int text_start_index = bh.text_start_address / sizeof(bin_instr_t);
    for (int i = 0; i < bh.text_length / sizeof(bin_instr_t); i++)
        memory.instrs[text_start_index + i] = instruction_read(bf);

    // Load data
    const int data_start_index = bh.data_start_address / sizeof(word_type);
    for (int i = 0; i < bh.data_length / sizeof(word_type); i++)
        memory.words[data_start_index + i] = bof_read_word(bf);

    // Set initial register values
    GPR[GP] = bh.data_start_address;
    GPR[FP] = GPR[SP] = bh.stack_bottom_addr;
    PC = bh.text_start_address;
}

// Executes loaded program and prints tracing ouput
void machine_exec()
{
    trace_state(GPR, memory, PC, HI, LO);
    trace_instr(PC, memory.instrs[PC]);
}