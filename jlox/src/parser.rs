use crate::{
    syntax::{Expr, LiteralType},
    token::{Token, TokenType},
};

/// Parses the tokens into expressions using Lox's grammar defined below:
/// expression -> equality ;
/// equality   -> comparison ( ( "!=" | "==" ) comparison )* ;
/// comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
/// term       -> factor ( ( "-" | "+" ) factor )* ;
/// factor     -> unary ( ( "/" | "*" ) unary )* ;
/// unary      -> ( "!" | "-" ) unary | primary ;
/// primary    -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
struct Parser {
    tokens: Vec<Token>,
    current: usize,
}

/// Error that will be reported when the Parser encounters something wrong.
struct ParserError {
    token: Token,
    message: &'static str,
}

/// Parses the given tokens and returns the resultant expression in the Some variant. If an error
/// occurred while parsing, the error will be reported and None will be returned.
pub fn parse(tokens: Vec<Token>) -> Option<Expr> {
    let mut parser = Parser { tokens, current: 0 };
    match parser.expression() {
        Ok(expr) => Some(expr),
        Err(e) => {
            e.report();
            None
        }
    }
}

/// Checks if the token current token is equal to any of the given types on $x. If so, it consumes
/// the token and returns true, or returns false without consuming.
macro_rules! matches {
    ( $sel:ident, $( $x:expr ),* ) => {
        {
            if $( $sel.check($x) )||* {
                $sel.advance();
                true
            } else {
                false
            }
        }
    };
}

impl Parser {
    fn expression(&mut self) -> Result<Expr, ParserError> {
        self.equality()
    }

    fn equality(&mut self) -> Result<Expr, ParserError> {
        let mut expr = self.comparison()?;
        while matches!(self, TokenType::BangEqual, TokenType::EqualEqual) {
            let operator: Token = (*self.previous()).clone();
            let right: Expr = self.comparison()?;
            expr = Expr::Binary {
                left: Box::new(expr),
                operator,
                right: Box::new(right),
            };
        }
        Ok(expr)
    }

    fn comparison(&mut self) -> Result<Expr, ParserError> {
        let mut expr = self.addition()?;
        while matches!(
            self,
            TokenType::Greater,
            TokenType::GreaterEqual,
            TokenType::Less,
            TokenType::LessEqual
        ) {
            let operator: Token = self.previous().clone();
            let right = self.addition()?;
            expr = Expr::Binary {
                left: Box::new(expr),
                operator,
                right: Box::new(right),
            }
        }
        Ok(expr)
    }

    fn addition(&mut self) -> Result<Expr, ParserError> {
        let mut expr = self.multiplication()?;
        while matches!(self, TokenType::Minus, TokenType::Plus) {
            let operator: Token = self.previous().clone();
            let right = self.multiplication()?;
            expr = Expr::Binary {
                left: Box::new(expr),
                operator,
                right: Box::new(right),
            }
        }
        Ok(expr)
    }

    fn multiplication(&mut self) -> Result<Expr, ParserError> {
        let mut expr = self.unary()?;
        while matches!(self, TokenType::Slash, TokenType::Star) {
            let operator: Token = self.previous().clone();
            let right = self.unary()?;
            expr = Expr::Binary {
                left: Box::new(expr),
                operator,
                right: Box::new(right),
            }
        }
        Ok(expr)
    }

    fn unary(&mut self) -> Result<Expr, ParserError> {
        if matches!(self, TokenType::Bang, TokenType::Minus) {
            let operator = self.previous().clone();
            let right = self.unary()?;
            Ok(Expr::Unary {
                operator,
                right: Box::new(right),
            })
        } else {
            self.primary()
        }
    }

    fn primary(&mut self) -> Result<Expr, ParserError> {
        let expr = match &self.peek().token_type {
            TokenType::False => Expr::Literal {
                value: LiteralType::Boolean(false),
            },
            TokenType::True => Expr::Literal {
                value: LiteralType::Boolean(true),
            },
            TokenType::Nil => Expr::Literal {
                value: LiteralType::Nil,
            },
            TokenType::String(s) => Expr::Literal {
                value: LiteralType::String(s.to_string()),
            },
            TokenType::Number(n) => Expr::Literal {
                value: LiteralType::Number(*n),
            },
            TokenType::LeftParen => {
                self.advance(); // so it doesn't keep finding the same LeftParen
                let expr = self.expression()?;
                self.consume(TokenType::RightParen, "Expected ')' after expression.")?;
                Expr::Grouping {
                    expression: Box::new(expr),
                }
            }
            _ => {
                return Err(ParserError {
                    token: self.peek().clone(),
                    message: "Expected expression.",
                })
            }
        };
        self.advance();
        Ok(expr)
    }

    fn synchronize(&mut self) {
        self.advance();
        while !self.is_at_end() {
            if self.previous().token_type == TokenType::Semicolon {
                return;
            }
            match self.peek().token_type {
                TokenType::Class
                | TokenType::Fun
                | TokenType::Var
                | TokenType::For
                | TokenType::If
                | TokenType::While
                | TokenType::Print
                | TokenType::Return => return,
                _ => self.advance(),
            };
        }
    }

    fn consume(
        &mut self,
        token_type: TokenType,
        message: &'static str,
    ) -> Result<&Token, ParserError> {
        if self.check(token_type) {
            Ok(self.advance())
        } else {
            Err(ParserError {
                token: self.peek().clone(),
                message,
            })
        }
    }

    fn peek(&self) -> &Token {
        self.tokens
            .get(self.current)
            .expect("Peek into end of token stream.")
    }

    fn previous(&self) -> &Token {
        self.tokens
            .get(self.current - 1)
            .expect("Previous token was empty.")
    }

    fn is_at_end(&self) -> bool {
        self.peek().token_type == TokenType::Eof
    }

    fn advance(&mut self) -> &Token {
        if !self.is_at_end() {
            self.current += 1;
        }
        self.previous()
    }

    fn check(&self, token_type: TokenType) -> bool {
        if self.is_at_end() {
            return false;
        }
        self.peek().token_type == token_type
    }
}

impl ParserError {
    fn report(&self) {
        match self.token.token_type {
            TokenType::Eof => {
                eprintln!("[line {}] Error at end: {}", self.token.line, self.message)
            }
            _ => eprintln!(
                "[line {}] Error at '{}': {}",
                self.token.line,
                self.token.lexeme(),
                self.message
            ),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::scanner;
    use crate::syntax::AstPrinter;

    #[test]
    fn test_parser() {
        let (tokens, had_error) = scanner::scan_tokens("-123 * 45.67");
        assert_eq!(had_error, false, "Failed to scan the test code.");
        let expression = parse(tokens).expect("Failed to parse the test code.");
        let printer = AstPrinter;
        assert_eq!(expression.accept(&printer), "(* (- 123) 45.67)");
    }
}
