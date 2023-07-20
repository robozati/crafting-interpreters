mod lox;
mod parser;
mod scanner;
mod syntax;
mod token;

use std::cmp;
use std::env;
use std::path::Path;
use std::process;

fn main() -> Result<(), lox::LoxError> {
    // Lox can start in an interactive mode if there is no argument, or parse a file specified in
    // the first argument
    let args: Vec<String> = env::args().collect();
    // 2 because the first argument is the program's name
    match args.len().cmp(&2) {
        cmp::Ordering::Greater => {
            // Exit with code EX_USAGE (64) if there is more than one argument
            println!("Usage: {} [script]", args[0]);
            process::exit(64);
        }
        cmp::Ordering::Equal => {
            let path = Path::new(&args[1]);
            lox::run_file(path)?;
        }
        cmp::Ordering::Less => lox::run_prompt()?,
    }
    Ok(())
}
