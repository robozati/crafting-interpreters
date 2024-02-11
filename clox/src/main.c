#include "chunk.h"
#include "debug.h"

int main(void) {
    Chunk chunk = chunk_new_alloc();

    size_t constant = chunk_add_constant(&chunk, 1.2);
    chunk_push(&chunk, OPCODE_constant, 123);
    chunk_push(&chunk, constant, 123);

    chunk_push(&chunk, OPCODE_return, 123);

    debug_disassemble_chunk(chunk, "test");
    chunk_free(&chunk);
    return 0;
}
