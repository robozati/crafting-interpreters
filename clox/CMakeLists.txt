cmake_minimum_required(VERSION 3.28)
project(clox LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS FALSE)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(${PROJECT_NAME} src/main.c)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -fsanitize=address
        -Wall
        -Wextra
        -pedantic
        -Wformat=2
        -Wshadow
        -Wwrite-strings
        -Wstrict-prototypes
        -Wold-style-definition
        -Wredundant-decls
        -Wnested-externs
        -Wmissing-include-dirs
)

target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)

add_subdirectory(src)
