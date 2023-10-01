#include <stdio.h>
#include "trace.h"

// Prints word values in memory over given range, omitting repeated zero values
// If end is -1, stops at the first zero value
void print_address_range(word_type words[], int start, int end)
{
    bool last_value_zero = false;
    int num_values_printed = 0;
    word_type value;
    for (int i = start; end == -1 || i < end; i += BYTES_PER_WORD)
    {
        value = words[i / BYTES_PER_WORD];
        // Repeated value of zero, so skip
        if (value == 0 && last_value_zero)
            continue;

        // Print address and value
        printf("%8d: %d", i, value);

        // Record whether this value was zero and print ellipses if necessary
        if (value == 0)
        {
            last_value_zero = true;
            printf("\t...");
        }
        else
        {
            last_value_zero = false;
        }

        printf(num_values_printed % 5 == 4 ? "\n" : "\t");
        num_values_printed++;

        if (value == 0 && end == -1)
            break;
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
        printf(i % 6 == 5 || i == NUM_REGISTERS - 1 ? "\n" : "\t");
    }
}

// Prints values between $gp and $sp (data section)
void trace_data(reg_type GPR[], mem_u memory)
{
    print_address_range(memory.words, GPR[GP], -1);
}

// Prints values between $sp and $fp (runtime stack)
void trace_stack(reg_type GPR[], mem_u memory)
{
    print_address_range(memory.words, GPR[SP], GPR[FP] + BYTES_PER_WORD);
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