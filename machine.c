#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"
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
void execute_syscall_instruction(unsigned int code)
{
    switch (code)
    {
    case exit_sc:
        exit(0);
        break;
    case print_str_sc:
        printf("%s", &memory.bytes[GPR[4]]);
        break;
    case print_char_sc:
        GPR[2] = fputc(GPR[4], stdout);
        break;
    case read_char_sc:
        GPR[2] = fgetc(stdin);
        break;
    case start_tracing_sc:
        tracing_active = true;
        break;
    case stop_tracing_sc:
        tracing_active = false;
        break;
    default:
        bail_with_error("Unknown code (%d) in execute_syscall_instruction", code);
        break;
    }
}

// Execute register/computational instruction (or syscall if func == SYSCALL_F)
// If func == SYSCALL_F, cascade to execute_syscall_instruction()
void execute_reg_instruction(bin_instr_t bi)
{
    long long int product;
    assert(bi.reg.op == REG_O);
    switch (bi.reg.func)
    {
    case ADD_F:
        GPR[bi.reg.rd] = GPR[bi.reg.rs] + GPR[bi.reg.rt];
        break;
    case SUB_F:
        GPR[bi.reg.rd] = GPR[bi.reg.rs] - GPR[bi.reg.rt];
        break;
    case MUL_F:
        product = (long long int)GPR[bi.reg.rs] * (long long int)GPR[bi.reg.rt];
        LO = (reg_type)product;
        HI = (reg_type)(product >> 32);
        break;
    case DIV_F:
        if (GPR[bi.reg.rt] == 0)
            bail_with_error("Divide by zero error");
        LO = GPR[bi.reg.rs] / GPR[bi.reg.rt];
        HI = GPR[bi.reg.rs] % GPR[bi.reg.rt];
        break;
    case MFHI_F:
        GPR[bi.reg.rd] = HI;
        break;
    case MFLO_F:
        GPR[bi.reg.rd] = LO;
        break;
    case AND_F:
        GPR[bi.reg.rd] = GPR[bi.reg.rs] & GPR[bi.reg.rt];
        break;
    case BOR_F:
        GPR[bi.reg.rd] = GPR[bi.reg.rs] | GPR[bi.reg.rt];
        break;
    case NOR_F:
        GPR[bi.reg.rd] = ~(GPR[bi.reg.rs] | GPR[bi.reg.rt]);
        break;
    case XOR_F:
        GPR[bi.reg.rd] = GPR[bi.reg.rs] ^ GPR[bi.reg.rt];
        break;
    case SLL_F:
        GPR[bi.reg.rd] = GPR[bi.reg.rt] << bi.reg.shift;
        break;
    case SRL_F:
        GPR[bi.reg.rd] = GPR[bi.reg.rt] >> bi.reg.shift;
        break;
    case JR_F:
        PC = GPR[bi.reg.rs];
        break;
    case SYSCALL_F:
        execute_syscall_instruction(instruction_syscall_number(bi));
        break;
    default:
        bail_with_error("Unknown function code (%d) in execute_reg_instruction",
                        bi.reg.func);
        break;
    }
}

// Execute immediate instruction
// If op == REG_O, cascade to execute_reg_instruction()
void execute_immed_instruction(bin_instr_t bi)
{
    switch (bi.immed.op)
    { // pretend it's an immediate instruction
    case REG_O:
        execute_reg_instruction(bi);
        break;
    case ADDI_O:
        GPR[bi.immed.rt] = GPR[bi.immed.rs] + machine_types_sgnExt(bi.immed.immed);
        break;
    case ANDI_O:
        GPR[bi.immed.rt] = GPR[bi.immed.rs] & machine_types_zeroExt(bi.immed.immed);
        break;
    case BORI_O:
        GPR[bi.immed.rt] = GPR[bi.immed.rs] | machine_types_zeroExt(bi.immed.immed);
        break;
    case XORI_O:
        GPR[bi.immed.rt] = GPR[bi.immed.rs] ^ machine_types_zeroExt(bi.immed.immed);
        break;
    case BEQ_O:
        if (GPR[bi.immed.rs] == GPR[bi.immed.rt])
            PC += machine_types_formOffset(bi.immed.immed);
        break;
    case BGEZ_O:
        if (GPR[bi.immed.rs] >= 0)
            PC += machine_types_formOffset(bi.immed.immed);
        break;
    case BGTZ_O:
        if (GPR[bi.immed.rs] > 0)
            PC += machine_types_formOffset(bi.immed.immed);
        break;
    case BLEZ_O:
        if (GPR[bi.immed.rs] <= 0)
            PC += machine_types_formOffset(bi.immed.immed);
        break;
    case BLTZ_O:
        if (GPR[bi.immed.rs] < 0)
            PC += machine_types_formOffset(bi.immed.immed);
        break;
    case BNE_O:
        if (GPR[bi.immed.rs] != GPR[bi.immed.rt])
            PC += machine_types_formOffset(bi.immed.immed);
        break;
    case LBU_O:
    {
        int index = GPR[bi.immed.rs] + machine_types_formOffset(bi.immed.immed);
        GPR[bi.immed.rt] = machine_types_zeroExt(memory.bytes[index]);
        break;
    }
    case LW_O:
    {
        int index = GPR[bi.immed.rs] + machine_types_formOffset(bi.immed.immed);
        GPR[bi.immed.rt] = memory.words[index / BYTES_PER_WORD];
        break;
    }
    case SB_O:
    {
        int index = GPR[bi.immed.rs] + machine_types_formOffset(bi.immed.immed);
        memory.bytes[index] = GPR[bi.immed.rt];
        break;
    }
    break;
    case SW_O:
    {
        int index = GPR[bi.immed.rs] + machine_types_formOffset(bi.immed.immed);
        memory.words[index / BYTES_PER_WORD] = GPR[bi.immed.rt];
        break;
    }
    break;
    case JMP_O:
        PC = machine_types_formAddress(PC, bi.jump.addr);
        break;
    case JAL_O:
        GPR[31] = PC;
        PC = machine_types_formAddress(PC, bi.jump.addr);
        break;
    default:
        bail_with_error("Unknown op code (%d) in execute_immed_instruction!",
                        bi.immed.op);
    }
}

void execute_instruction(bin_instr_t IR)
{
    execute_immed_instruction(IR);
}

// Enforces required invariants
void check_invariants()
{
    assert(PC % BYTES_PER_WORD == 0);
    assert(GPR[GP] % BYTES_PER_WORD == 0);
    assert(GPR[SP] % BYTES_PER_WORD == 0);
    assert(GPR[FP] % BYTES_PER_WORD == 0);
    assert(0 <= GPR[GP]);
    assert(GPR[GP] < GPR[SP]);
    assert(GPR[SP] <= GPR[FP]);
    assert(GPR[FP] < MEMORY_SIZE_IN_BYTES);
    assert(0 <= PC);
    assert(PC < MEMORY_SIZE_IN_BYTES);
    assert(GPR[0] == 0);
}

// Executes loaded program and prints tracing ouput
void machine_exec()
{
    bin_instr_t IR;
    while (true)
    {
        check_invariants();
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