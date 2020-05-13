#include "urm_simulator.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum instruction_type {
    ZERO,
    SUCCESSOR,
    TRANSFER,
    JUMP
} instruction_type_t;

typedef struct instruction {
    instruction_type_t type;
    int arg1;
    int arg2;
    int arg3;
} instruction_t;

static int *registers;
static int num_registers;
static int next_instruction;

int read_register(int reg) {
    reg -= 1;
    return reg >= num_registers ? 0 : registers[reg];
}

void set_register(int reg, int value) {
    reg -= 1;
    registers[reg] = value;
}

void print_registers() {
    for (int i = 0; i < num_registers; i++) {
        printf("%d, ", registers[i]);
    }
    printf("0, 0, 0, ...\n");
}

void initialize_registers(int *initial_registers, int num_initial, int num_to_allocate) {
    if (num_initial > num_to_allocate) {
        num_to_allocate = num_initial;
    }
    registers = calloc(num_to_allocate, sizeof(int));
    num_registers = num_to_allocate;
    for (int i = 0; i < num_initial; i++) {
        registers[i] = initial_registers[i];
    }
}

void zero(int reg) {
    set_register(reg, 0);
    next_instruction += 1;
}

void successor(int reg) {
    int value = read_register(reg);
    set_register(reg, value + 1);
    next_instruction += 1;
}

void transfer(int destination, int source) {
    int value = read_register(source);
    set_register(destination, value);
    next_instruction += 1;
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

void initialize_instruction(instruction_t *instruction, instruction_type_t type, int arg1, int arg2, int arg3) {
    instruction->type = type;
    instruction->arg1 = arg1;
    instruction->arg2 = arg2;
    instruction->arg3 = arg3;
}

void print_instruction(instruction_t *instruction, int instruction_number) {
    printf("Instruction %d: ", instruction_number);
    switch (instruction->type) {
        case ZERO:
            printf("Z(%d)\n", instruction->arg1);
            break;
        case SUCCESSOR:
            printf("S(%d)\n", instruction->arg1);
            break;
        case TRANSFER:
            printf("T(%d, %d)\n", instruction->arg1, instruction->arg2);
            break;
        case JUMP:
            printf("J(%d, %d, %d)\n", instruction->arg1, instruction->arg2, instruction->arg3);
            break;
    }
}

void execute_instruction(instruction_t *instruction) {
    switch (instruction->type) {
        case ZERO:
            zero(instruction->arg1);
            break;
        case SUCCESSOR:
            successor(instruction->arg1);
            break;
        case TRANSFER:
            transfer(instruction->arg1, instruction->arg2);
            break;
        case JUMP:
            jump(instruction->arg1, instruction->arg2, instruction->arg3);
            break;
    }
}

void print_instructions(instruction_t **instructions, int num_instructions) {
    for (int i = 0; i < num_instructions; i++) {
        print_instruction(instructions[i], i + 1);
    }
    printf("\n");
}

int parse_arg(char *instruction, char **arg_end) {
    while (*instruction == ' ' || *instruction == '\t') {
        instruction++;
    }

    char* offset = instruction;

    while ('0' <= *instruction && *instruction <= '9') {
        instruction++;
    }

    *arg_end = instruction;

    int arg = 0;
    int exp = 1;
    instruction--;

    while (instruction >= offset) {
        arg += exp * (*instruction - '0');
        exp *= 10;
        instruction--;
    }

    if (**arg_end == ' ' || **arg_end == '\t') {
        (*arg_end)++;
    }

    return arg;
}

instruction_t * parse_instruction(char *instruction, char **instruction_end, int program_line) {
    if (instruction[1] != '(') {
        printf("\nInvalid instruction on line: %d\n\n", program_line);
        return NULL;
    }

    instruction_t *parsed_instruction = malloc(sizeof(instruction_t));
    parsed_instruction->arg1 = -1;

    switch (instruction[0]) {
        case 'Z':
        case 'z': {
            int reg = parse_arg(instruction + 2, &instruction);
            if (*instruction != ')') {
                printf("\nInvalid instruction on line: %d\n\n", program_line);
                break;
            }
            initialize_instruction(parsed_instruction, ZERO, reg, -1, -1);
            break;
        }
        case 'S':
        case 's': {
            int reg = parse_arg(instruction + 2, &instruction);
            if (*instruction != ')') {
                printf("\nInvalid instruction on line: %d\n\n", program_line);
                break;
            }
            initialize_instruction(parsed_instruction, SUCCESSOR, reg, -1, -1);
            break;
        }
        case 'T':
        case 't': {
            int destination = parse_arg(instruction + 2, &instruction);
            if (*instruction != ',') {
                printf("\nInvalid instruction on line: %d\n\n", program_line);
                break;
            }
            int source = parse_arg(instruction + 1, &instruction);
            if (*instruction != ')') {
                printf("\nInvalid instruction on line: %d\n\n", program_line);
                return NULL;
            }
            initialize_instruction(parsed_instruction, TRANSFER, destination, source, -1);
            break;
        }
        case 'J':
        case 'j': {
            int reg1 = parse_arg(instruction + 2, &instruction);
            if (*instruction != ',') {
                printf("\nInvalid instruction on line: %d\n\n", program_line);
                break;
            }
            int reg2 = parse_arg(instruction + 1, &instruction);
            if (*instruction != ',') {
                printf("\nInvalid instruction on line: %d\n\n", program_line);
                break;
            }
            int jump_instruction = parse_arg(instruction + 1, &instruction);
            if (*instruction != ')') {
                printf("\nInvalid instruction on line: %d\n\n", program_line);
                break;
            }
            initialize_instruction(parsed_instruction, JUMP, reg1, reg2, jump_instruction);
            break;
        }
        default:
            printf("\nInvalid instruction on line: %d\n\n", program_line);
    }

    if (parsed_instruction->arg1 == -1) {
        free(parsed_instruction);
        return NULL;
    }

    instruction++;

    while (*instruction == ' ' || *instruction  == '\t') {
        instruction++;
    }

    if (*instruction != '#' && *instruction != '\n' && *instruction != '\0') {
        printf("Error on line: %d\nInstruction can only be followed by newline or comment!\n", program_line);
        free(parsed_instruction);
        return NULL;
    }

    *instruction_end = instruction;
    return parsed_instruction;
}

int parse_instructions(char *program, instruction_t ***instructions, int *num_registers) {
    int max_instructions = 1;
    *instructions = malloc(max_instructions * sizeof(instruction_t *));

    int curr_instruction = 0;
    int curr_line = 1;
    int max_register_number = 0;

    while (*program != '\0') {
        // ignore comments and blank lines
        if (*program != '#' && *program != '\n') {
            // parse label
            char *instruction = program;
            if ('0' <= *instruction && *instruction <= '9') {
                char *label_end;
                int label = strtol(instruction, &label_end, 10);
                if (label != curr_instruction + 1) {
                    printf("Label (%d) doesn't correspond to instruction number (%d)\n",
                           label, curr_instruction + 1);
                }
                if (*label_end != ':') {
                    printf("Invalid label on line %d! \n", curr_line);
                    return -1;
                } else {
                    instruction = label_end;
                    instruction++;
                    while ((*instruction == ' ' || *instruction == '\t') && *instruction != '\0') {
                        instruction++;
                    }
                }
                program = instruction;
            }

            // parse instruction and check if it's valid
            instruction_t *parsed_instruction = parse_instruction(program, &program, curr_line);
            if (parsed_instruction == NULL) {
                for (int i = 0; i < curr_instruction; i++) {
                    free((*instructions)[i]);
                }
                free(*instructions);
                return -1;
            }

            if (parsed_instruction->arg1 > max_register_number) {
                max_register_number = parsed_instruction->arg1;
            }

            if (curr_instruction == max_instructions) {
                instruction_t **new_instructions = malloc(2 * max_instructions * sizeof(instruction_t *));
                for (int i = 0; i < max_instructions; i++) {
                    new_instructions[i] = (*instructions)[i];
                }
                max_instructions *= 2;
                free(*instructions);
                *instructions = new_instructions;
            }

            (*instructions)[curr_instruction] = parsed_instruction;
            curr_instruction++;
        }

        // the line is a comment
        while (*program != '\n' && *program != '\0') {
            program++;
        }

        // skip blank lines
        while (*program == '\n' && *program != '\0') {
            curr_line++;
            program++;
        }
    }
    *num_registers = max_register_number;
    return curr_instruction;
}

int run(char *program, int *initial_registers, int num_initial_registers) {
    num_registers = 0;
    next_instruction = 0;

    int num_registers_to_allocate;
    instruction_t **instructions = NULL;

    int num_instructions = parse_instructions(program, &instructions, &num_registers_to_allocate);
    if (num_instructions < 1) {
        return 1;
    }

    initialize_registers(initial_registers, num_initial_registers, num_registers_to_allocate);

    printf("Executing_program:\n");
    print_instructions(instructions, num_instructions);

    printf("Initial registers: ");
    print_registers();
    printf("\n");

    while (next_instruction < num_instructions) {
        print_instruction(instructions[next_instruction], next_instruction + 1);
        print_registers();
        execute_instruction(instructions[next_instruction]);
        print_registers();
        printf("\n");
    }

    printf("Result: %d\n\n", registers[0]);

    free(registers);

    return 0;
}
