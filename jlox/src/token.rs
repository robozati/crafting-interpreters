use std::fmt::Display;

/// Bundled lexeme with information around it.
pub struct Token<'a> {
    token_type: TokenType,
    pub lexeme: &'a str,
    line: usize,
}

impl<'a> Token<'a> {
    pub fn new(token_type: TokenType, lexeme: &'a str, line: usize) -> Token {
        Token {
            token_type,
            lexeme,
            line,
        }
    }
}

impl Display for Token<'_> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match &self.token_type {
            TokenType::String(literal) => write!(f, "String {} {}", self.lexeme, literal),
            TokenType::Number(literal) => write!(f, "Number {} {}", self.lexeme, literal),
            _ => write!(f, "{:?} {}", self.token_type, self.lexeme),
        }
    }
}

/// Type of the token. All the types match a language feature.
#[derive(Debug)]
pub enum TokenType {
    // Single-character tokens
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Comma,
    Dot,
    Minus,
    Plus,
    Semicolon,
    Slash,
    Star,
    // One or two character tokens
    Bang,
    BangEqual,
    Equal,
    EqualEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,
    // Literals
    Identifier,
    String(String),
    Number(f64),
    // Keywords
    And,
    Class,
    Else,
    False,
    Fun,
    For,
    If,
    Nil,
    Or,
    Print,
    Return,
    Super,
    This,
    True,
    Var,
    While,
    // EOF
    Eof,
}
