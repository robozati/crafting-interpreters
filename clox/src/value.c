#include "value.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

ValueVector value_vector_new_alloc(void) {
    Value* values = malloc(sizeof(*values) * CLOX_VALUE_VECTOR_MIN_CAPACITY);
    assert(values != NULL);
    return (ValueVector){.values = values,
                         .capacity = CLOX_VALUE_VECTOR_MIN_CAPACITY,
                         .count = 0};
}

void value_vector_free(ValueVector* pValueVector) {
    assert(pValueVector != NULL);
    assert(pValueVector->values != NULL);
    free(pValueVector->values);
    pValueVector->values = NULL;
}

ValueVector value_vector_push(ValueVector const values_vector,
                              Value const value) {
    size_t new_capacity = values_vector.capacity;
    Value* new_values = values_vector.values;
    assert(values_vector.values != NULL);
    // Size full, needs to reallocate
    if (values_vector.count >= values_vector.capacity) {
        new_capacity = (values_vector.capacity < CLOX_VALUE_VECTOR_MIN_CAPACITY
                            ? CLOX_VALUE_VECTOR_MIN_CAPACITY
                            : values_vector.capacity * 2);
        new_values = realloc(values_vector.values,
                             sizeof(*values_vector.values) * new_capacity);
        assert(new_values != NULL);
    }
    new_values[values_vector.count] = value;
    return (ValueVector){.values = new_values,
                         .capacity = new_capacity,
                         .count = values_vector.count + 1};
}

void value_print(const Value value) { printf("%g", value); }
