#ifndef CLOX_VM_H
#define CLOX_VM_H

#include <stdbool.h>
#include <stdint.h>

#include "chunk.h" // Chunk
#include "value.h" // Value

#define STACK_MAX 256

typedef struct {
    Chunk chunk;
    uint8_t* ip;
    Value* stack;
    Value* stack_top;
} VirtualMachine;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

InterpretResult vm_interpret(Chunk const chunk, bool const is_debug);

#endif // !CLOX_VM_H
