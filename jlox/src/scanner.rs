use crate::token::{Token, TokenType};

/// Errors that can happen in the scanning process. The usize variable is to annotate the line
/// number.
enum ScannerError {
    UnexpectedCharacter(usize),
    UnterminatedString(usize),
}

/// Struct responsible for scanning the source code and generating tokens.
/// This object should not be instantiated and its only reason is for grouping all the data and helper
/// functions in scan_tokens()
pub struct Scanner<'a> {
    source: &'a str,
    tokens: Vec<Token>,
    start: usize,
    current: usize,
    line: usize,
}

/// Scans the source code and returns a vector with the tokens and a bool indicating if an error occurred.
/// The scanner doesn't stop parsing the tokens if there was an error due to the book's implementation.
pub fn scan_tokens(source: &str) -> (Vec<Token>, bool) {
    let mut had_error = false;
    let mut scanner = Scanner::new(source);
    while !scanner.is_at_end() {
        scanner.start = scanner.current;
        if let Err(e) = scanner.scan_token() {
            match e {
                ScannerError::UnterminatedString(line) => {
                    eprintln!("[line {}] Error: Unterminated string.", line);
                }
                ScannerError::UnexpectedCharacter(line) => {
                    eprintln!("[line {}] Error: Unexpected character.", line);
                }
            }
            had_error = true;
        }
    }
    scanner.tokens.push(Token {
        token_type: TokenType::Eof,
        line: scanner.line,
    });
    (scanner.tokens, had_error)
}

// There's only one public function, scan_tokens(), the rest are helpers.
impl<'a> Scanner<'a> {
    fn new(source: &'a str) -> Self {
        Self {
            source,
            tokens: Vec::new(),
            start: 0,
            current: 0,
            line: 1,
        }
    }

    fn scan_token(&mut self) -> Result<(), ScannerError> {
        let c = self.advance();
        match c {
            '(' => self.add_token(TokenType::LeftParen),
            ')' => self.add_token(TokenType::RightParen),
            '{' => self.add_token(TokenType::LeftBrace),
            '}' => self.add_token(TokenType::RightBrace),
            ',' => self.add_token(TokenType::Comma),
            '.' => self.add_token(TokenType::Dot),
            '-' => self.add_token(TokenType::Minus),
            '+' => self.add_token(TokenType::Plus),
            ';' => self.add_token(TokenType::Semicolon),
            '*' => self.add_token(TokenType::Star),
            '!' => {
                let token_type = if self.is_expected('=') {
                    TokenType::BangEqual
                } else {
                    TokenType::Bang
                };
                self.add_token(token_type)
            }
            '=' => {
                let token_type = if self.is_expected('=') {
                    TokenType::EqualEqual
                } else {
                    TokenType::Equal
                };
                self.add_token(token_type)
            }
            '<' => {
                let token_type = if self.is_expected('=') {
                    TokenType::LessEqual
                } else {
                    TokenType::Less
                };
                self.add_token(token_type)
            }
            '>' => {
                let token_type = if self.is_expected('=') {
                    TokenType::GreaterEqual
                } else {
                    TokenType::Greater
                };
                self.add_token(token_type)
            }
            '/' => {
                // Comment until the end of the line
                if self.is_expected('/') {
                    while !self.is_at_end() && self.peek() != '\n' {
                        self.advance();
                    }
                } else {
                    self.add_token(TokenType::Slash)
                }
            }
            ' ' | '\r' | '\t' => {} // Ignore whitespace
            '\n' => self.line += 1,
            '"' => self.string()?,
            c => {
                if c.is_ascii_digit() {
                    self.number();
                } else if c.is_alphabetic() {
                    self.identifier();
                } else {
                    return Err(ScannerError::UnexpectedCharacter(self.line));
                }
            }
        };
        Ok(())
    }

    fn is_at_end(&self) -> bool {
        self.current >= self.source.len()
    }

    fn peek(&self) -> char {
        // We can call unwrap here since we won't encounter a EOF
        self.source.chars().nth(self.current).unwrap_or('\0')
    }

    fn peek_next(&self) -> char {
        self.source.chars().nth(self.current + 1).unwrap_or('\0')
    }

    fn advance(&mut self) -> char {
        let c = self.peek();
        self.current += 1;
        c
    }

    fn is_expected(&mut self, expected: char) -> bool {
        if self.is_at_end() || self.peek() != expected {
            return false;
        }
        self.current += 1;
        true
    }

    fn add_token(&mut self, token_type: TokenType) {
        self.tokens.push(Token {
            token_type,
            line: self.line,
        });
    }

    fn string(&mut self) -> Result<(), ScannerError> {
        while !self.is_at_end() && self.peek() != '"' {
            if self.peek() == '\n' {
                self.line += 1;
            }
            self.advance();
        }
        if self.is_at_end() {
            return Err(ScannerError::UnterminatedString(self.line));
        }
        // Advances for the closing '"'
        self.advance();
        // Trim the surrouding quotes
        let value = &self.source[self.start + 1..self.current - 1];
        self.tokens.push(Token {
            token_type: TokenType::String(value.to_string()),
            line: self.line,
        });
        Ok(())
    }

    fn number(&mut self) {
        while self.peek().is_ascii_digit() {
            self.advance();
        }
        // Look for a fractional part
        if self.peek() == '.' && self.peek_next().is_ascii_digit() {
            self.advance(); // Consume the '.'
            while self.peek().is_ascii_digit() {
                self.advance();
            }
        }
        self.add_token(TokenType::Number(
            self.source[self.start..self.current]
                .parse::<f64>()
                .unwrap(),
        ))
    }

    fn identifier(&mut self) {
        while self.peek().is_alphanumeric() {
            self.advance();
        }
        let text = &self.source[self.start..self.current];
        let token_type = match text {
            "and" => TokenType::And,
            "class" => TokenType::Class,
            "else" => TokenType::Else,
            "false" => TokenType::False,
            "for" => TokenType::For,
            "fun" => TokenType::Fun,
            "if" => TokenType::If,
            "nil" => TokenType::Nil,
            "or" => TokenType::Or,
            "print" => TokenType::Print,
            "return" => TokenType::Return,
            "super" => TokenType::Super,
            "this" => TokenType::This,
            "true" => TokenType::True,
            "var" => TokenType::Var,
            "while" => TokenType::While,
            _ => TokenType::Identifier(text.to_string()),
        };
        self.add_token(token_type);
    }
}
