#ifndef CLOX_CHUNK_H
#define CLOX_CHUNK_H

#include <stddef.h>
#include <stdint.h>

#include "line.h"  // LineVector
#include "value.h" // Value, ValueVector

#define CLOX_CHUNK_MIN_CAPACITY 8

enum OPCODE { OPCODE_constant, OPCODE_return };

typedef struct {
    uint8_t* code;
    size_t capacity;
    size_t count;
    ValueVector constants;
    LineVector line_vector;
} Chunk;

Chunk chunk_new_alloc(void) __attribute__((warn_unused_result));

void chunk_free(Chunk* pChunk);

void chunk_push(Chunk* pChunk, uint8_t const byte, int const line);

size_t chunk_add_constant(Chunk* pChunk, Value const value);

#endif // !CLOX_CHUNK_H
