#include <stdio.h>
#include "trace.h"

// Prints word values in memory over given range, omitting repeated zero values
void print_address_range(word_type words[], int start, int end, size_t size)
{
    bool last_value_zero = false;
    int num_values_printed = 0;
    for (int i = start; i < end; i += size)
    {
        // Repeated value of zero, so skip
        if (words[i] == 0 && last_value_zero)
            continue;

        // Print address and value
        printf("%8d: %d", i, words[i]);
        printf(num_values_printed % 5 == 4 ? "\n" : "\t");
        num_values_printed++;

        // Record whether this value was zero and print ellipses if necessary
        if (words[i] == 0)
        {
            last_value_zero = true;
            printf("...");
        }
        else
        {
            last_value_zero = false;
        }
    }
    if (num_values_printed % 5 != 0)
        printf("\n");
}

// Prints values of special purpose registers
void trace_special(address_type PC, reg_type HI, reg_type LO)
{
    // Value of PC
    printf("%8s: %d", "PC", PC);
    // Values of HI and LO if they're non-zero
    if (HI != 0 || LO != 0)
        printf("\t%8s: %d\t%8s: %d\n", "HI", HI, "LO", LO);
    else
        printf("\n");
}

// Prints values of GPR
void trace_gpr(reg_type GPR[])
{
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        printf("GPR[%-3s]: %-4d", regname_get(i), GPR[i]);
        printf(i % 6 == 5 ? "\n" : "\t");
    }
    if (NUM_REGISTERS % 6 != 0)
        printf("\n");
}

// Prints values between $gp and $sp (data section)
void trace_data(reg_type GPR[], mem_u memory)
{
    print_address_range(memory.words, GPR[GP], GPR[SP], sizeof(word_type));
}

// Prints values between $sp and $fp (runtime stack)
void trace_stack(reg_type GPR[], mem_u memory)
{
    print_address_range(memory.words, GPR[SP], GPR[FP] + sizeof(word_type), sizeof(word_type));
}

// Prints state of registers and memory
void trace_state(reg_type GPR[], mem_u memory, address_type PC, reg_type HI, reg_type LO)
{
    trace_special(PC, HI, LO);
    trace_gpr(GPR);
    trace_data(GPR, memory);
    trace_stack(GPR, memory);
}

// Prints address and assembly form of instruction
void trace_instr(address_type PC, bin_instr_t bi)
{
    printf("==> addr: %4d %s\n", PC, instruction_assembly_form(bi));
}