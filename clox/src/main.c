#include <stdbool.h>
#include <string.h>

#include "chunk.h" // Chunk, chunk_*
#include "vm.h"    // vm_*

int main(int argc, char* argv[]) {
    bool is_debug = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            is_debug = true;
        }
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

    vm_interpret(chunk, is_debug);

    chunk_free(&chunk);
    return 0;
}
