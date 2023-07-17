use crate::scanner::Scanner;
use std::{
    fs,
    io::{self, Write},
    path::Path,
};

/// Highest error that is returned by the application. It's used in `main()`.
#[derive(Debug)]
pub enum LoxError {
    IoError(std::io::Error),
    ScanningError,
}

/// Reads the file and executes it.
pub fn run_file(path: &Path) -> Result<(), LoxError> {
    let content = fs::read_to_string(path)?;
    run(&content)?;
    Ok(())
}

/// Starts an interactive prompt where it's possible to execute one line at a time.
pub fn run_prompt() -> Result<(), LoxError> {
    loop {
        print!("> ");
        io::stdout().flush()?;
        let mut buf = String::new();
        io::stdin().read_line(&mut buf)?;
        // buf is empty when it reaches EOF. Type CTRL-D to invoke a EOF
        if buf.is_empty() {
            // Prints a newline so the console doesn't stop after "> "
            println!();
            break;
        }
        // The REPL should not terminate after encountering an error, so we are ignoring it
        let _ = run(&buf);
    }
    Ok(())
}

fn run(source: &str) -> Result<(), LoxError> {
    let (tokens, had_error) = Scanner::scan_tokens(source);
    if !had_error {
        for token in tokens {
            println!("{}", token);
        }
        return Ok(());
    }
    Err(LoxError::ScanningError)
}

impl std::fmt::Display for LoxError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::IoError(e) => e.fmt(f),
            Self::ScanningError => write!(f, "Could not execute due to an error while scanning."),
        }
    }
}

impl From<std::io::Error> for LoxError {
    fn from(value: std::io::Error) -> Self {
        Self::IoError(value)
    }
}
