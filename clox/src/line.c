#include "line.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

LineVector line_vector_new_alloc(void) {
    LineInfo* lines = malloc(sizeof(*lines) * CLOX_LINE_VECTOR_MIN_CAPACITY);
    assert(lines != NULL);
    return (LineVector){
        .lines = lines, .capacity = CLOX_LINE_VECTOR_MIN_CAPACITY, .count = 0};
}

void line_vector_free(LineVector* pLineVector) {
    assert(pLineVector != NULL);
    assert(pLineVector->lines != NULL);
    free(pLineVector->lines);
    pLineVector->lines = NULL;
}

LineVector line_vector_push(LineVector const line_vector,
                            LineInfo const line_info) {
    size_t new_capacity = line_vector.capacity;
    LineInfo* new_lines = line_vector.lines;
    assert(line_vector.lines != NULL);
    // Size full, needs to reallocate
    if (line_vector.count >= line_vector.capacity) {
        new_capacity = (line_vector.capacity < CLOX_LINE_VECTOR_MIN_CAPACITY
                            ? CLOX_LINE_VECTOR_MIN_CAPACITY
                            : line_vector.capacity * 2);
        new_lines = realloc(line_vector.lines,
                            sizeof(*line_vector.lines) * new_capacity);
        assert(new_lines != NULL);
    }
    new_lines[line_vector.count] = line_info;
    return (LineVector){.lines = new_lines,
                        .capacity = new_capacity,
                        .count = line_vector.count + 1};
}

int line_vector_get_line(LineVector const line_vector,
                         size_t const instruction) {
    size_t start = 0;
    size_t end = line_vector.count - 1;
    for (;;) {
        size_t mid = (start + end) / 2;
        LineInfo* line = &line_vector.lines[mid];
        if (instruction < line->offset) {
            end = mid - 1;
        } else if (mid == line_vector.count - 1 ||
                   instruction < line_vector.lines[mid + 1].offset) {
            return line->line;
        } else {
            start = mid + 1;
        }
    }
}
