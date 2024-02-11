#include "compiler.h"

#include <stdio.h>

#include "scanner.h" // Scanner, scanner_*
#include "token.h"   // Token

void compiler_compile(char const* const source) {
    Scanner scanner = scanner_new(source);
    int line = -1;
    for (;;) {
        Token token = scanner_scan_token(&scanner);
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF)
            break;
    }
}
