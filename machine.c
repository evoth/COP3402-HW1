#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine.h"
#include "machine_types.h"
#include "trace.h"
#include "utilities.h"

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

// Whether tracing is currently activated
bool tracing_active = true;

// Initialize machine state from binary object file
void machine_init(const char *filename)
{
    // Open binary object file and read header
    bf = bof_read_open(filename);
    bh = bof_read_header(bf);

    // Initialize memory and registers to 0
    memset(&memory, 0, bh.stack_bottom_addr + BYTES_PER_WORD);
    memset(GPR, 0, BYTES_PER_WORD * NUM_REGISTERS);

    // Load instructions
    const int text_start_index = bh.text_start_address / BYTES_PER_WORD;
    for (int i = 0; i < bh.text_length / BYTES_PER_WORD; i++)
        memory.instrs[text_start_index + i] = instruction_read(bf);

    // Load data
    const int data_start_index = bh.data_start_address / BYTES_PER_WORD;
    for (int i = 0; i < bh.data_length / BYTES_PER_WORD; i++)
        memory.words[data_start_index + i] = bof_read_word(bf);

    // Set initial register values
    GPR[GP] = bh.data_start_address;
    GPR[FP] = GPR[SP] = bh.stack_bottom_addr;
    PC = bh.text_start_address;
}

// Execute the syscall that corresponds to the given code
void execute_syscall(unsigned int code)
{
    switch (code)
    {
    case exit_sc:
        // return "EXIT";
        exit(0);
        break;
    case print_str_sc:
        // return "PSTR";
        break;
    case print_char_sc:
        // return "PCH";
        break;
    case read_char_sc:
        // return "RCH";
        break;
    case start_tracing_sc:
        // return "STRA";
        tracing_active = true;
        break;
    case stop_tracing_sc:
        // return "NOTR";
        tracing_active = false;
        break;
    default:
        bail_with_error("Unknown code (%d) in execute_syscall", code);
        break;
    }
}

// Execute register/computational instruction (or syscall if func == SYSCALL_F)
// If func == SYSCALL_F, cascade to execute_syscall()
void execute_func_instruction(bin_instr_t bi)
{
    assert(bi.reg.op == REG_O);
    switch (bi.reg.func)
    {
    case ADD_F:
        // return "ADD";
        break;
    case SUB_F:
        // return "SUB";
        break;
    case MUL_F:
        // return "MUL";
        break;
    case DIV_F:
        // return "DIV";
        break;
    case MFHI_F:
        // return "MFHI";
        break;
    case MFLO_F:
        // return "MFLO";
        break;
    case AND_F:
        // return "AND";
        break;
    case BOR_F:
        // return "BOR";
        break;
    case NOR_F:
        // return "NOR";
        break;
    case XOR_F:
        // return "XOR";
        break;
    case SLL_F:
        // return "SLL";
        break;
    case SRL_F:
        // return "SRL";
        break;
    case JR_F:
        // return "JR";
        break;
    case SYSCALL_F:
        execute_syscall(instruction_syscall_number(bi));
        break;
    default:
        bail_with_error("Unknown function code (%d) in execute_func_instruction",
                        bi.reg.func);
        break;
    }
}

// Execute immediate instruction
// If op == REG_O, cascade to execute_func_instruction()
void execute_immed_instruction(bin_instr_t bi)
{
    switch (bi.immed.op)
    { // pretend it's an immediate instruction
    case REG_O:
        execute_func_instruction(bi);
        break;
    case ADDI_O:
        // return "ADDI";
        GPR[bi.immed.rt] = GPR[bi.immed.rs] + machine_types_sgnExt(bi.immed.immed);
        break;
    case ANDI_O:
        // return "ANDI";
        break;
    case BORI_O:
        // return "BOI";
        break;
    case XORI_O:
        // return "XORI";
        break;
    case BEQ_O:
        // return "BEQ";
        break;
    case BGEZ_O:
        // return "BGEZ";
        break;
    case BGTZ_O:
        // return "BGTZ";
        break;
    case BLEZ_O:
        // return "BLEZ";
        break;
    case BLTZ_O:
        // return "BLTZ";
        break;
    case BNE_O:
        // return "BNE";
        break;
    case LBU_O:
        // return "LBU";
        break;
    case LW_O:
        // return "LW";
        break;
    case SB_O:
        // return "SB";
        break;
    case SW_O:
        // return "SW";
        break;
    case JMP_O:
        // return "JMP";
        break;
    case JAL_O:
        // return "JAL";
        break;
    default:
        bail_with_error("Unknown op code (%d) in instruction_mnemonic!",
                        bi.immed.op);
    }
}

void execute_instruction(bin_instr_t IR)
{
    execute_immed_instruction(IR);
}

// Executes loaded program and prints tracing ouput
void machine_exec()
{
    bin_instr_t IR;
    while (true)
    {
        if (tracing_active)
        {
            trace_state(GPR, memory, PC, HI, LO);
            trace_instr(PC, memory.instrs[PC / BYTES_PER_WORD]);
        }
        IR = memory.instrs[PC / BYTES_PER_WORD];
        PC += BYTES_PER_WORD;
        execute_instruction(IR);
    }
}

// Prints assembly instruction listing and initial data values
void machine_print_program()
{
    printf("Addr Instruction\n");
    const int text_start_index = bh.text_start_address / BYTES_PER_WORD;
    int instr_address;
    for (int i = 0; i < bh.text_length / BYTES_PER_WORD; i++)
    {
        instr_address = (text_start_index + i) * BYTES_PER_WORD;
        printf("%4d %s\n", instr_address,
               instruction_assembly_form(memory.instrs[text_start_index + i]));
    }
    trace_data(GPR, memory);
}