#include <stdio.h>
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

void machine_init(const char *filename)
{
    bf = bof_read_open(filename);
    bh = bof_read_header(bf);
    printf("%d\n", bh.stack_bottom_addr);
}