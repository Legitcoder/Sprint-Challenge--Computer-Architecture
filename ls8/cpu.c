#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

unsigned char cpu_ram_read(struct cpu *cpu)
{
    return cpu->ram[++cpu->pc];
}


/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *filename)
{
  FILE *file = fopen(filename, "r");

  if(file == NULL) {
      printf("Error opening file\n");
      return;
  }

  char line[100];

  int address = 0;

  while(fgets(line, 100, file)) {
    if(line[0] == '#') continue;
    long l = strtol(line, NULL, 2);
    cpu->ram[address++] = l;
  }

  fclose(file);
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char *regA, unsigned char *regB)
{
  switch (op) {
      case ALU_MUL:
          *regA *= *regB;
          break;
      case ALU_ADD:
          *regA += *regB;
          break;
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction
  cpu->registers[7] = 244;
  unsigned char top = cpu->registers[7];

  while (running) {
    unsigned char command = cpu->ram[cpu->pc];
    int operand_one, operand_two;

    switch(command) {
        case LDI:
            operand_one = cpu_ram_read(cpu);
            operand_two = cpu_ram_read(cpu);
            cpu->registers[operand_one] = (unsigned char)operand_two;
            cpu->pc++;
            break;
        case PRN:
            operand_one = cpu_ram_read(cpu);
            printf("%d\n", cpu->registers[operand_one]);
            cpu->pc++;
            break;
        case CALL:
            operand_one = cpu_ram_read(cpu);
            unsigned int address = cpu->registers[operand_one];
            unsigned int saved = ++cpu->pc;
            cpu->ram[--top] = (unsigned char)saved;
            cpu->pc = address;
            break;
        case MUL:
            operand_one = cpu_ram_read(cpu);
            operand_two = cpu_ram_read(cpu);
            alu(cpu, ALU_MUL, &cpu->registers[operand_one], &cpu->registers[operand_two]);
            cpu->pc++;
            break;
        case ADD:
            operand_one = cpu_ram_read(cpu);
            operand_two = cpu_ram_read(cpu);
            alu(cpu, ALU_ADD, &cpu->registers[operand_one], &cpu->registers[operand_two]);
            cpu->pc++;
            break;
        case PUSH:
            operand_one = cpu_ram_read(cpu);
            int reg_int = cpu->registers[operand_one];
            cpu->ram[--top] = (unsigned char)reg_int;
            cpu->pc++;
            break;
        case RET:
            cpu->pc = cpu->ram[top++];
            break;
        case POP:
            operand_one = cpu_ram_read(cpu);
            cpu->registers[operand_one] = cpu->ram[top++];
            cpu->pc++;
            break;
        case HLT:
            printf("HALTED\n");
            running = 0;
            break;
        default:
            printf("Unrecognized Instruction\n");
            exit(1);
    }
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  cpu->pc = 0;
  memset(cpu->registers, '0',  8 * sizeof(char));
  memset(cpu->ram, '0',  256 * sizeof(char));
}