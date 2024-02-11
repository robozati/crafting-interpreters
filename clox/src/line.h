#ifndef CLOX_LINE_H
#define CLOX_LINE_H

#include <stddef.h>

#define CLOX_LINE_VECTOR_MIN_CAPACITY 8

typedef struct {
    int line;
    size_t offset;
} LineInfo;

typedef struct {
    LineInfo* lines;
    size_t count;
    size_t capacity;
} LineVector;

LineVector line_vector_new_alloc(void) __attribute__((warn_unused_result));

void line_vector_free(LineVector* pLineVector);

LineVector line_vector_push(LineVector const line_vector,
                            LineInfo const line_info)
    __attribute__((warn_unused_result));

int line_vector_get_line(LineVector const line_vector,
                         size_t const instruction);

#endif // !CLOX_LINE_H
