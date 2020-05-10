#include <stdio.h>
#include <stdlib.h>

#include "urm_simulator.h"

static int num_registers = 0;
static int *registers;
static int next_instruction = 0;

int read_register(int reg) {
    reg -= 1;
    return reg >= num_registers ? 0 : registers[reg];
}

void set_register(int reg, int value) {
    reg -= 1;
    if (reg >= num_registers) {
        int *new_registers = calloc(2 * num_registers, sizeof(int));
        for (int i = 0; i < num_registers; i++) {
            new_registers[i] = registers[i];
        }
        free(registers);
        registers = new_registers;
        num_registers *= 2;
    }
    registers[reg] = value;
}

void print_registers() {
    for (int i = 0; i < num_registers; i++) {
        printf("%d, ", registers[i]);
    }
    printf("0, 0, 0, ...\n");
}

void zero(int reg) {
    set_register(reg, 0);
}

void successor(int reg) {
    int value = read_register(reg);
    set_register(reg, value + 1);
}

void transfer(int destination, int source) {
    int value = read_register(source);
    set_register(destination, value);
}

void jump(int reg1, int reg2, int instruction) {
    int value1 = read_register(reg1);
    int value2 = read_register(reg2);
    if (value1 == value2) {
        next_instruction = instruction - 1;
    } else {
        next_instruction += 1;
    }
}

int parse_arg(char *instruction, char **end) {
    int i = 0;

    while (instruction[i] == ' ' || instruction[i] == '\t') {
        i++;
    }

    int offset = i;

    while ('0' <= instruction[i] && instruction[i] <= '9') {
        i++;
    }

    *end = instruction + i;

    int arg = 0;
    int exp = 1;
    i -= 1;

    while (i >= offset) {
        arg += exp * (instruction[i] - '0');
        exp *= 10;
        i--;
    }

    if (**end == ' ' || **end == '\t') {
        *end += 1;
    }

    return arg;
}

int execute_instruction(char *instruction) {
    if (instruction[1] != '(') {
        printf("\nInvalid instruction: '%d: %s'\n", next_instruction + 1, instruction);
        return 1;
    }

    switch (instruction[0]) {
        case 'Z':
        case 'z': {
            char *end = NULL;
            int reg = parse_arg(instruction + 2, &end);
            if (*end != ')') {
                printf("\nInvalid instruction: '%d: %s'\n", next_instruction + 1, instruction);
                return 1;
            }
            printf("\nInstruction %d: %s\n", next_instruction, instruction);
            print_registers();
            zero(reg);
            print_registers();
            break;
        }
        case 'S':
        case 's': {
            char *end;
            int reg = parse_arg(instruction + 2, &end);
            if (*end != ')') {
                printf("\nInvalid instruction: '%d: %s'\n", next_instruction + 1, instruction);
                return 1;
            }
            printf("\nInstruction %d: %s\n", next_instruction + 1, instruction);
            print_registers();
            successor(reg);
            print_registers();
            break;
        }
        case 'T':
        case 't': {
            char *end;
            int destination = parse_arg(instruction + 2, &end);
            if (*end != ',') {
                printf("\nInvalid instruction: '%d: %s'\n", next_instruction + 1, instruction);
                return 1;
            }
            int source = parse_arg(end + 1, &end);
            if (*end != ')') {
                printf("\nInvalid instruction: '%d: %s'\n", next_instruction + 1, instruction);
                return 1;
            }
            printf("\nInstruction %d: %s\n", next_instruction + 1, instruction);
            print_registers();
            transfer(destination, source);
            print_registers();
            break;
        }
        case 'J':
        case 'j': {
            char *end;
            int reg1 = parse_arg(instruction + 2, &end);
            if (*end != ',') {
                printf("\nInvalid instruction: '%d: %s'\n", next_instruction + 1, instruction);
                return 1;
            }
            int reg2 = parse_arg(end + 1, &end);
            if (*end != ',') {
                printf("\nInvalid instruction: '%d: %s'\n", next_instruction + 1, instruction);
                return 1;
            }
            int jump_instruction = parse_arg(end + 1, &end);
            if (*end != ')') {
                printf("\nInvalid instruction: '%d: %s'\n", next_instruction + 1, instruction);
                return 1;
            }
            printf("\nInstruction %d: %s\n", next_instruction + 1, instruction);
            print_registers();
            jump(reg1, reg2, jump_instruction);
            print_registers();
            return 0;
        }
        default:
            printf("\nUnknown instruction: '%d: %s' ", next_instruction + 1, instruction);
            return 1;
    }

    next_instruction += 1;
    return 0;
}

void initialize_registers(int *initial_registers, int size) {
    registers = malloc(size * sizeof(int));
    num_registers = size;
    for (int i = 0; i < size; i++) {
        registers[i] = initial_registers[i];
    }
    printf("Initial registers: ");
    print_registers();
}

int run(char **program, int num_instructions, int *initial_registers, int number_of_initial_registers) {
    initialize_registers(initial_registers, number_of_initial_registers);

    while (next_instruction < num_instructions) {
        int status = execute_instruction(program[next_instruction]);
        if (status != 0) {
            return status;
        }
    }

    printf("\nResult: %d\n\n", registers[0]);

    return 0;
}
