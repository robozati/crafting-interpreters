use crate::token::Token;

/// Lox's expression grammar.
/// The Expr needs to be boxed in certain values because Expr is an enum that references itself,
/// and this can be recursive, so the size is not known at compile time.
pub enum Expr<'a> {
    Literal {
        value: LiteralType,
    },
    Grouping {
        expression: Box<Expr<'a>>,
    },
    Unary {
        operator: Token<'a>,
        right: Box<Expr<'a>>,
    },
    Binary {
        left: Box<Expr<'a>>,
        operator: Token<'a>,
        right: Box<Expr<'a>>,
    },
}

/// Type of the literal. The grammar is:
/// literal -> NUMBER | STRING | "true" | "false" | "nil";
pub enum LiteralType {
    Number(f64),
    String(String),
    Boolean(bool),
    Nil,
}

/// Visitor pattern that will be implemented by each expression.
/// The rationale behind this is that, by using the accept() function on Expr, we already have a
/// match of what visitor function we will run, so we can implement these functions to change its
/// return type.
pub trait Visitor<R> {
    fn visit_binary_expr(&self, left: &Expr, operator: &Token, right: &Expr) -> R;
    fn visit_grouping_expr(&self, expression: &Expr) -> R;
    fn visit_literal_expr(&self, value: &LiteralType) -> R;
    fn visit_unary_expr(&self, operator: &Token, right: &Expr) -> R;
}

impl<'a> Expr<'a> {
    /// Function to run in each expression. See the Visitor design pattern for more details.
    pub fn accept<R>(&self, visitor: &dyn Visitor<R>) -> R {
        match self {
            Expr::Literal { value } => visitor.visit_literal_expr(value),
            Expr::Grouping { expression } => visitor.visit_grouping_expr(expression),
            Expr::Unary { operator, right } => visitor.visit_unary_expr(operator, right),
            Expr::Binary {
                left,
                operator,
                right,
            } => visitor.visit_binary_expr(left, operator, right),
        }
    }
}

/// Pretty prints the AST tree.
pub struct AstPrinter;

impl AstPrinter {
    fn parenthesize(&self, name: &str, exprs: Vec<&Expr>) -> String {
        let mut s = String::new();
        s.push('(');
        s.push_str(name);
        for expr in exprs {
            s.push(' ');
            s.push_str(&expr.accept::<String>(self));
        }
        s.push(')');
        s
    }
}

impl Visitor<String> for AstPrinter {
    fn visit_binary_expr(&self, left: &Expr, operator: &Token, right: &Expr) -> String {
        self.parenthesize(operator.lexeme, vec![left, right])
    }

    fn visit_grouping_expr(&self, expr: &Expr) -> String {
        self.parenthesize("group", vec![expr])
    }

    fn visit_unary_expr(&self, operator: &Token, right: &Expr) -> String {
        self.parenthesize(operator.lexeme, vec![right])
    }

    fn visit_literal_expr(&self, value: &LiteralType) -> String {
        match value {
            // We need to run .to_string() after every value instead of after the match because
            // Number returns a f64, so the match arms would have different return types.
            LiteralType::Number(value) => value.to_string(),
            LiteralType::String(value) => value.to_string(),
            LiteralType::Nil => "nil".to_string(),
            LiteralType::Boolean(value) => {
                if *value {
                    "true".to_string()
                } else {
                    "false".to_string()
                }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::token::{Token, TokenType};

    #[test]
    fn test_printer() {
        let expression = Expr::Binary {
            left: Box::new(Expr::Unary {
                operator: Token::new(TokenType::Minus, "-", 1),
                right: Box::new(Expr::Literal {
                    value: LiteralType::Number(123.0),
                }),
            }),
            operator: Token::new(TokenType::Star, "*", 1),
            right: Box::new(Expr::Grouping {
                expression: Box::new(Expr::Literal {
                    value: LiteralType::Number(45.67),
                }),
            }),
        };
        let printer = AstPrinter;
        assert_eq!(expression.accept(&printer), "(* (- 123) (group 45.67))");
    }
}
