#include "compiler.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"   // Chunk, chunk_*
#include "scanner.h" // Scanner, scanner_*
#include "token.h"   // Token
#include "value.h"   // Value

#define CLOX_DEBUG_PRINT_CODE

#ifdef CLOX_DEBUG_PRINT_CODE
#include "debug.h" // debug_*
#endif

typedef struct {
    Token current;
    Token previous;
    bool had_error;
    bool panic_mode;
    Chunk* chunk;
    Scanner* scanner;
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT, // =
    PREC_OR,         // or
    PREC_AND,        // and
    PREC_EQUALITY,   // == !=
    PREC_COMPARISON, // < > <= >=
    PREC_TERM,       // + -
    PREC_FACTOR,     // * /
    PREC_UNARY,      // ! -
    PREC_CALL,       // . ()
    PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(Parser*);

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

// Forward declaration of rules
static ParseRule rules[];

static ParseRule* get_rule(TokenType type) { return &rules[type]; }

static void error_at(Parser* parser, Token const* token, char const* message) {
    if (parser->panic_mode) {
        return;
    }
    parser->panic_mode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing.
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser->had_error = true;
}

static void error_at_current(Parser* parser, char const* message) {
    error_at(parser, &parser->current, message);
}

static void error(Parser* parser, char const* message) {
    error_at(parser, &parser->previous, message);
}

static void advance(Parser* parser) {
    parser->previous = parser->current;
    for (;;) {
        parser->current = scanner_scan_token(parser->scanner);
        if (parser->current.type != TOKEN_ERROR) {
            break;
        }
        error_at_current(parser, parser->current.start);
    }
}

static void consume(Parser* parser, TokenType type, char const* message) {
    if (parser->current.type == type) {
        advance(parser);
        return;
    }
    error_at_current(parser, message);
}

static void emit_byte(Parser const* parser, uint8_t byte) {
    chunk_push(parser->chunk, byte, parser->previous.line);
}

static void emit_bytes(Parser const* parser, uint8_t byte1, uint8_t byte2) {
    emit_byte(parser, byte1);
    emit_byte(parser, byte2);
}

static void emit_return(Parser const* parser) {
    emit_byte(parser, OPCODE_return);
}

static void end_compiler(Parser const* parser) {
    emit_return(parser);
#ifdef CLOX_DEBUG_PRINT_CODE
    if (parser->had_error) {
        debug_disassemble_chunk(parser->chunk, "code");
    }
#endif
}

static void parsePrecedence(Parser* parser, Precedence precedence) {
    advance(parser);
    ParseFn prefix_rule = get_rule(parser->previous.type)->prefix;
    if (prefix_rule == NULL) {
        error(parser, "Expect expression.");
        return;
    }
    prefix_rule(parser);
    while (precedence <= get_rule(parser->current.type)->precedence) {
        advance(parser);
        ParseFn infix_rule = get_rule(parser->previous.type)->infix;
        infix_rule(parser);
    }
}

static void expression(Parser* parser) {
    parsePrecedence(parser, PREC_ASSIGNMENT);
}

static uint8_t make_constant(Parser* parser, Value value) {
    size_t constant = chunk_add_constant(parser->chunk, value);
    if (constant > UINT8_MAX) {
        error(parser, "Too many constants in one chunk.");
        return 0;
    }
    return (uint8_t)constant;
}

static void emit_constant(Parser* parser, Value value) {
    emit_bytes(parser, OPCODE_constant, make_constant(parser, value));
}

static void number(Parser* parser) {
    double value = strtod(parser->previous.start, NULL);
    emit_constant(parser, value);
}

static void grouping(Parser* parser) {
    expression(parser);
    consume(parser, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void unary(Parser* parser) {
    TokenType operator_type = parser->previous.type;
    // Compile the operand.
    parsePrecedence(parser, PREC_UNARY);
    // Emit the operator instruction.
    switch (operator_type) {
    case TOKEN_MINUS:
        emit_byte(parser, OPCODE_negate);
        break;
    default:
        return; // Unreachable.
    }
}

static void binary(Parser* parser) {
    TokenType operator_type = parser->previous.type;
    ParseRule* rule = get_rule(operator_type);
    parsePrecedence(parser, (Precedence)(rule->precedence + 1));

    switch (operator_type) {
    case TOKEN_PLUS:
        emit_byte(parser, OPCODE_add);
        break;
    case TOKEN_MINUS:
        emit_byte(parser, OPCODE_subtract);
        break;
    case TOKEN_STAR:
        emit_byte(parser, OPCODE_multiply);
        break;
    case TOKEN_SLASH:
        emit_byte(parser, OPCODE_divide);
        break;
    default:
        return; // Unreachable.
    }
}

static ParseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG] = {NULL, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_GREATER] = {NULL, NULL, PREC_NONE},
    [TOKEN_GREATER_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_LESS] = {NULL, NULL, PREC_NONE},
    [TOKEN_LESS_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
    [TOKEN_STRING] = {NULL, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {NULL, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {NULL, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

bool compiler_compile(char const* source, Chunk* chunk) {
    Scanner scanner = scanner_new(source);
    Parser parser = {.had_error = false,
                     .panic_mode = false,
                     .chunk = chunk,
                     .scanner = &scanner};
    advance(&parser);
    expression(&parser);
    consume(&parser, TOKEN_EOF, "Expect end of expression.");
    end_compiler(&parser);
    return !parser.had_error;
}
