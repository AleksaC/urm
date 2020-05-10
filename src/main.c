#include "urm_simulator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("You need to provide path to your program!\n");
        return 1;
    }

    char *program_path = argv[1];
    FILE *program_source = fopen(program_path, "r");

    if (program_source == NULL) {
        printf("%s is not a path to an existing file!\n", program_path);
        return 1;
    }

    fseek(program_source, 0, SEEK_END);
    long program_size = ftell(program_source);
    fseek(program_source, 0, SEEK_SET);

    char *program = malloc(program_size + 1);
    if (fread(program, 1, program_size, program_source) != program_size) {
        printf("Something went wrong while reading the file %s\n", program_path);
        return 1;
    }
    program[program_size] = '\0';

    fclose(program_source);

    int max_instructions = 1;
    char **instructions = malloc(max_instructions * sizeof(char *));

    int curr_instruction = 0;
    int curr_line = 1;

    while (*program != '\0') {
        if (*program != '#' && *program != '\n') {
            if (curr_instruction == max_instructions) {
                max_instructions *= 2;
                char **new_instructions = malloc(max_instructions * sizeof(char *));
                for (int i = 0; i < max_instructions; i++) {
                    new_instructions[i] = instructions[i];
                }
                free(instructions);
                instructions = new_instructions;
            }
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
                    return 1;
                } else {
                    instruction = label_end;
                    instruction++;
                    while ((*instruction == ' ' || *instruction == '\t') && *instruction != '\0') {
                        instruction++;
                    }
                }
                program = instruction;
            }
            if (*instruction != 'z' && *instruction != 'Z' && *instruction != 's' &&
                *instruction != 'S' && *instruction != 't' && *instruction != 'T' &&
                *instruction != 'j' && *instruction != 'J') {
                printf("Invalid instruction on line %d!\n", curr_line);
                return 1;
            }
            while (*program != '\n' && *program != '\0') {
                program++;
            }
            int instruction_len = program - instruction;
            instructions[curr_instruction] = malloc((instruction_len + 1) * sizeof(char *));
            strncpy(instructions[curr_instruction], instruction, instruction_len);
            instructions[curr_instruction][instruction_len] = '\0';
            curr_instruction++;
        }
        while (*program != '\n' && *program != '\0') {
            program++;
        }
        while (*program == '\n' && *program != '\0') {
            curr_line++;
            program++;
        }
    }

    int initial_registers[argc - 2];

    for (int i = 2; i < argc; i++) {
        char *end;
        int value = strtol(argv[i], &end, 10);
        if (value == 0 && *end != '\0') {
            printf("'%s' is not a valid register value! Registers can only "
                   "contain natural numbers!\n", argv[i]);
            return 1;
        }
        initial_registers[i - 2] = value;
    }

    return run(instructions, curr_instruction, initial_registers, argc - 2);
}
