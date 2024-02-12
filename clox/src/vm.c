#include "vm.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "chunk.h"    // Chunk, OPCODE_*
#include "compiler.h" // compiler_*
#include "value.h"    // Value, value_*

#define CLOX_DEBUG_TRACE_EXECUTION

#ifdef CLOX_DEBUG_TRACE_EXECUTION
#include "debug.h" // debug_*
#endif

static void push(VirtualMachine* pVm, Value value) {
    assert(pVm != NULL);
    assert(pVm->stack_top < pVm->stack + STACK_MAX);
    *pVm->stack_top = value;
    pVm->stack_top += 1;
}

static Value pop(VirtualMachine* pVm) {
    assert(pVm != NULL);
    assert(pVm->stack_top >= pVm->stack);
    pVm->stack_top -= 1;
    return *pVm->stack_top;
}

static InterpretResult run(VirtualMachine vm) {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk.constants.values[READ_BYTE()])
#define BINARY_OP(op)                                                          \
    do {                                                                       \
        double b = pop(&vm);                                                   \
        double a = pop(&vm);                                                   \
        push(&vm, a op b);                                                     \
    } while (false)

    for (;;) {
#ifdef CLOX_DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = vm.stack; slot < vm.stack_top; slot++) {
            printf("[ ");
            value_print(*slot);
            printf(" ]");
        }
        printf("\n");
        debug_disassemble_instruction(&vm.chunk, vm.ip - vm.chunk.code);
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
        case OPCODE_constant: {
            Value constant = READ_CONSTANT();
            push(&vm, constant);
            break;
        }
        case OPCODE_add:
            BINARY_OP(+);
            break;
        case OPCODE_subtract:
            BINARY_OP(-);
            break;
        case OPCODE_multiply:
            BINARY_OP(*);
            break;
        case OPCODE_divide:
            BINARY_OP(/);
            break;
        case OPCODE_negate:
            push(&vm, -pop(&vm));
            break;
        case OPCODE_return: {
            value_print(pop(&vm));
            printf("\n");
            return INTERPRET_OK;
        }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult vm_interpret(char const* const source) {
    Chunk chunk = chunk_new_alloc();

    if (!compiler_compile(source, &chunk)) {
        chunk_free(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    Value stack[STACK_MAX];
    VirtualMachine vm = {
        .chunk = chunk, .ip = chunk.code, .stack = stack, .stack_top = stack};

    InterpretResult result = run(vm);

    chunk_free(&chunk);
    return result;
}
