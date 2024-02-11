#include "scanner.h"

#include <stdbool.h>
#include <string.h>

#include "token.h" // Token, TokenType

Scanner scanner_new(char const* const source) {
    return (Scanner){.start = source, .current = source, .line = 1};
}

static bool is_at_end(Scanner const* pScanner) {
    return *pScanner->current == '\0';
}

static char advance(Scanner* pScanner) {
    pScanner->current += 1;
    return pScanner->current[-1];
}

static bool match(Scanner* pScanner, char expected) {
    if (is_at_end(pScanner) || *pScanner->current != expected) {
        return false;
    }
    pScanner->current += 1;
    return true;
}

static char peek(Scanner* pScanner) { return *pScanner->current; }

static char peek_next(Scanner* pScanner) {
    if (is_at_end(pScanner)) {
        return '\0';
    }
    return pScanner->current[1];
}

static void skipWhitespace(Scanner* pScanner) {
    for (;;) {
        char c = peek(pScanner);
        switch (c) {
        case ' ':
        case '\r':
        case '\t':
            advance(pScanner);
            break;
        case '\n':
            pScanner->line += 1;
            advance(pScanner);
            break;
        case '/':
            if (peek_next(pScanner) == '/') {
                // A comment goes until the end of the line.
                while (peek(pScanner) != '\n' && !is_at_end(pScanner)) {
                    advance(pScanner);
                }
            } else {
                return;
            }
            break;
        default:
            return;
        }
    }
}

static Token make_token(Scanner const* pScanner, TokenType type) {
    return (Token){.type = type,
                   .start = pScanner->start,
                   .length = (int)(pScanner->current - pScanner->start),
                   .line = pScanner->line};
}

static Token error_token(Scanner const* pScanner, char const* message) {
    return (Token){.type = TOKEN_ERROR,
                   .start = message,
                   .length = (int)strlen(message),
                   .line = pScanner->line};
}

static Token string(Scanner* pScanner) {
    while (peek(pScanner) != '"' && !is_at_end(pScanner)) {
        if (peek(pScanner) == '\n') {
            pScanner->line += 1;
        }
        advance(pScanner);
    }
    if (is_at_end(pScanner)) {
        return error_token(pScanner, "Unterminated string.");
    }
    // The closing quote.
    advance(pScanner);
    return make_token(pScanner, TOKEN_STRING);
}

static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static Token number(Scanner* pScanner) {
    while (is_digit(peek(pScanner))) {
        advance(pScanner);
    }
    // Look for a fractional part.
    if (peek(pScanner) == '.' && is_digit(peek_next(pScanner))) {
        // Consume the "."
        advance(pScanner);
        while (is_digit(peek(pScanner))) {
            advance(pScanner);
        }
    }
    return make_token(pScanner, TOKEN_NUMBER);
}

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static TokenType check_keyword(Scanner const* pScanner, int start, int length,
                               const char* rest, TokenType type) {
    if (pScanner->current - pScanner->start == start + length &&
        memcmp(pScanner->start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifier_type(Scanner const* pScanner) {
    switch (pScanner->start[0]) {
    case 'a':
        return check_keyword(pScanner, 1, 2, "nd", TOKEN_AND);
    case 'c':
        return check_keyword(pScanner, 1, 4, "lass", TOKEN_CLASS);
    case 'e':
        return check_keyword(pScanner, 1, 3, "lse", TOKEN_ELSE);
    case 'f':
        if (pScanner->current - pScanner->start > 1) {
            switch (pScanner->start[1]) {
            case 'a':
                return check_keyword(pScanner, 2, 3, "lse", TOKEN_FALSE);
            case 'o':
                return check_keyword(pScanner, 2, 1, "r", TOKEN_FOR);
            case 'u':
                return check_keyword(pScanner, 2, 1, "n", TOKEN_FUN);
            }
        }
        break;
    case 'i':
        return check_keyword(pScanner, 1, 1, "f", TOKEN_IF);
    case 'n':
        return check_keyword(pScanner, 1, 2, "il", TOKEN_NIL);
    case 'o':
        return check_keyword(pScanner, 1, 1, "r", TOKEN_OR);
    case 'p':
        return check_keyword(pScanner, 1, 4, "rint", TOKEN_PRINT);
    case 'r':
        return check_keyword(pScanner, 1, 5, "eturn", TOKEN_RETURN);
    case 's':
        return check_keyword(pScanner, 1, 4, "uper", TOKEN_SUPER);
    case 't':
        if (pScanner->current - pScanner->start > 1) {
            switch (pScanner->start[1]) {
            case 'h':
                return check_keyword(pScanner, 2, 2, "is", TOKEN_THIS);
            case 'r':
                return check_keyword(pScanner, 2, 2, "ue", TOKEN_TRUE);
            }
        }
        break;
    case 'v':
        return check_keyword(pScanner, 1, 2, "ar", TOKEN_VAR);
    case 'w':
        return check_keyword(pScanner, 1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

static Token identifier(Scanner* pScanner) {
    while (is_alpha(peek(pScanner)) || is_digit(peek(pScanner))) {
        advance(pScanner);
    }
    return make_token(pScanner, identifier_type(pScanner));
}

Token scanner_scan_token(Scanner* pScanner) {
    skipWhitespace(pScanner);
    pScanner->start = pScanner->current;
    if (is_at_end(pScanner)) {
        return make_token(pScanner, TOKEN_EOF);
    }

    char c = advance(pScanner);
    if (is_alpha(c)) {
        return identifier(pScanner);
    } else if (is_digit(c)) {
        return number(pScanner);
    }

    switch (c) {
    case '(':
        return make_token(pScanner, TOKEN_LEFT_PAREN);
    case ')':
        return make_token(pScanner, TOKEN_RIGHT_PAREN);
    case '{':
        return make_token(pScanner, TOKEN_LEFT_BRACE);
    case '}':
        return make_token(pScanner, TOKEN_RIGHT_BRACE);
    case ';':
        return make_token(pScanner, TOKEN_SEMICOLON);
    case ',':
        return make_token(pScanner, TOKEN_COMMA);
    case '.':
        return make_token(pScanner, TOKEN_DOT);
    case '-':
        return make_token(pScanner, TOKEN_MINUS);
    case '+':
        return make_token(pScanner, TOKEN_PLUS);
    case '/':
        return make_token(pScanner, TOKEN_SLASH);
    case '*':
        return make_token(pScanner, TOKEN_STAR);
    case '!':
        return make_token(pScanner,
                          match(pScanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
        return make_token(pScanner, match(pScanner, '=') ? TOKEN_EQUAL_EQUAL
                                                         : TOKEN_EQUAL);
    case '<':
        return make_token(pScanner,
                          match(pScanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
        return make_token(pScanner, match(pScanner, '=') ? TOKEN_GREATER_EQUAL
                                                         : TOKEN_GREATER);
    case '"':
        return string(pScanner);
    }

    return error_token(pScanner, "Unexpected character.");
}
