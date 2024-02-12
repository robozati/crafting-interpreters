#include "debug.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "chunk.h" // Chunk, OPCODE_*
#include "line.h"  // line_vector_*
#include "value.h" // value_*

static size_t simple_instruction(char const* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static size_t constant_instruction(char const* name, Chunk const* chunk,
                                   size_t offset) {
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    value_print(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

void debug_disassemble_chunk(Chunk const* chunk, char const* name) {
    printf("== %s == \n", name);
    for (size_t offset = 0; offset < chunk->count;) {
        offset = debug_disassemble_instruction(chunk, offset);
    }
}

size_t debug_disassemble_instruction(Chunk const* chunk, size_t offset) {
    printf("%04zu ", offset);
    int line = line_vector_get_line(chunk->line_vector, offset);
    if (offset > 0 &&
        line == line_vector_get_line(chunk->line_vector, offset - 1)) {
        printf("   | ");
    } else {
        printf("%4d ", line);
    }
    uint8_t const instruction = chunk->code[offset];
    switch (instruction) {
    case OPCODE_constant:
        return constant_instruction("OP_CONSTANT", chunk, offset);
    case OPCODE_add:
        return simple_instruction("OP_ADD", offset);
    case OPCODE_subtract:
        return simple_instruction("OP_SUBTRACT", offset);
    case OPCODE_multiply:
        return simple_instruction("OP_MULTIPLY", offset);
    case OPCODE_divide:
        return simple_instruction("OP_DIVIDE", offset);
    case OPCODE_negate:
        return simple_instruction("OP_NEGATE", offset);
    case OPCODE_return:
        return simple_instruction("OP_RETURN", offset);
    default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
}
