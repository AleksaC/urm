#include "urm_simulator.h"

#include <stdio.h>
#include <stdlib.h>

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
    size_t bytes_read = fread(program, 1, program_size, program_source);
    if (bytes_read != program_size && ftell(program_source) != program_size) {
        printf("Something went wrong while reading the file %s\n", program_path);
        return 1;
    }
    program[bytes_read] = '\0';

    fclose(program_source);

    int num_initial_registers = argc - 2;
    int initial_registers[num_initial_registers];

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

    return run(program, initial_registers, num_initial_registers);
}
