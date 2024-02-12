#ifndef CLOX_DEBUG_H
#define CLOX_DEBUG_H

#include <stddef.h>

#include "chunk.h" // Chunk

void debug_disassemble_chunk(Chunk const* chunk, char const* name);

size_t debug_disassemble_instruction(Chunk const* chunk, size_t offset);

#endif // !CLOX_DEBUG_H
