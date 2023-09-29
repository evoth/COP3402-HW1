#include <stdio.h>
#include <string.h>
#include "bof.h"
#include "machine.h"
#include "machine_types.h"

// Type of register
// TODO: should this be signed or unsigned?
typedef unsigned int reg_type;

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

// Initializes virtual machine state given filename of BOF.
void machine_init(const char *filename)
{
    // Open binary object file and read header
    bf = bof_read_open(filename);
    bh = bof_read_header(bf);

    // Initialize memory and registers
    memset(&memory, 0, bh.stack_bottom_addr);
    memset(GPR, 0, sizeof(reg_type) * NUM_REGISTERS);

    // Load instructions
    const int text_start_index = bh.text_start_address / sizeof(word_type);
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