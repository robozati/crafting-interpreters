#ifndef CLOX_SCANNER_H
#define CLOX_SCANNER_H

#include "token.h" // Token

typedef struct {
    char const* start;
    char const* current;
    int line;
} Scanner;

Scanner scanner_new(char const* const source);

Token scanner_scan_token(Scanner* pScanner);

#endif // !CLOX_SCANNER_H
