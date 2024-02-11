#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chunk.h" // Chunk, chunk_*
#include "vm.h"    // vm_*

#define MAX_LINE_SIZE 1024

static void repl(void) {
    char line[MAX_LINE_SIZE];
    for (;;) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        vm_interpret(line);
    }
}

static char* read_file(char const* const path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);
    assert(buffer != NULL);

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    assert(bytes_read == file_size);

    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}

static void run_file(char const* const path) {
    char* source = read_file(path);
    InterpretResult result = vm_interpret(source);

    free(source);
    source = NULL;

    if (result == INTERPRET_COMPILE_ERROR)
        exit(65);
    if (result == INTERPRET_RUNTIME_ERROR)
        exit(70);
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }

    Chunk chunk = chunk_new_alloc();

    size_t constant = chunk_add_constant(&chunk, 1.2);
    chunk_push(&chunk, OPCODE_constant, 123);
    chunk_push(&chunk, constant, 123);

    constant = chunk_add_constant(&chunk, 3.4);
    chunk_push(&chunk, OPCODE_constant, 123);
    chunk_push(&chunk, constant, 123);

    chunk_push(&chunk, OPCODE_add, 123);

    constant = chunk_add_constant(&chunk, 5.6);
    chunk_push(&chunk, OPCODE_constant, 123);
    chunk_push(&chunk, constant, 123);

    chunk_push(&chunk, OPCODE_divide, 123);
    chunk_push(&chunk, OPCODE_negate, 123);

    chunk_push(&chunk, OPCODE_return, 123);

    // vm_interpret(chunk);

    chunk_free(&chunk);
    return 0;
}
