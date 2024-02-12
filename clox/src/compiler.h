#ifndef CLOX_COMPILER_H
#define CLOX_COMPILER_H

#include <stdbool.h>

#include "chunk.h" // Chunk

bool compiler_compile(char const* source, Chunk* chunk);

#endif // !CLOX_COMPILER_H
