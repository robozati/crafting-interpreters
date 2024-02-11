#include "chunk.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "line.h"  // LineVector, LineInfo, line_*
#include "value.h" // Value, ValueVector, value_*

Chunk chunk_new_alloc(void) {
    uint8_t* code = malloc(sizeof(*code) * CLOX_CHUNK_MIN_CAPACITY);
    assert(code != NULL);
    ValueVector constants = value_vector_new_alloc();
    LineVector lines = line_vector_new_alloc();
    return (Chunk){.code = code,
                   .capacity = CLOX_CHUNK_MIN_CAPACITY,
                   .count = 0,
                   .constants = constants,
                   .line_vector = lines};
}

void chunk_free(Chunk* pChunk) {
    assert(pChunk != NULL);
    assert(pChunk->code != NULL);
    free(pChunk->code);
    pChunk->code = NULL;
    value_vector_free(&(pChunk->constants));
    line_vector_free(&(pChunk->line_vector));
}

void chunk_push(Chunk* pChunk, uint8_t const byte, int const line) {
    assert(pChunk != NULL);
    assert(pChunk->code != NULL);
    // Chunk full, needs to reallocate
    if (pChunk->count >= pChunk->capacity) {
        size_t new_capacity = (pChunk->capacity < CLOX_CHUNK_MIN_CAPACITY
                                   ? CLOX_CHUNK_MIN_CAPACITY
                                   : pChunk->capacity * 2);
        pChunk->code =
            realloc(pChunk->code, sizeof(*pChunk->code) * new_capacity);
        assert(pChunk->code != NULL);
    }
    pChunk->code[pChunk->count] = byte;
    pChunk->count += 1;
    // See if we're still on the same line.
    assert(pChunk->line_vector.lines != NULL);
    if (pChunk->line_vector.count > 0 &&
        pChunk->line_vector.lines[pChunk->line_vector.count - 1].line == line) {
        return;
    }
    // Append a new LineStart.
    LineInfo const line_info = {.offset = pChunk->count - 1, .line = line};
    pChunk->line_vector = line_vector_push(pChunk->line_vector, line_info);
}

size_t chunk_add_constant(Chunk* pChunk, Value const value) {
    assert(pChunk != NULL);
    ValueVector new_constants = value_vector_push(pChunk->constants, value);
    pChunk->constants = new_constants;
    return pChunk->constants.count - 1;
}
