#ifndef CLOX_VALUE_H
#define CLOX_VALUE_H

#include <stddef.h>

#define CLOX_VALUE_VECTOR_MIN_CAPACITY 8

typedef double Value;

typedef struct {
    Value* values;
    size_t count;
    size_t capacity;
} ValueVector;

ValueVector value_vector_new_alloc(void) __attribute__((warn_unused_result));

void value_vector_free(ValueVector* pValueVector);

ValueVector value_vector_push(ValueVector const values_vector,
                              Value const value)
    __attribute__((warn_unused_result));

void value_print(Value const value);

#endif // !CLOX_VALUE_H
