cmake_minimum_required(VERSION 3.26.3)
project(challenge03)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_executable(${PROJECT_NAME} src/main.c)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -Wextra
        -pedantic
)

add_subdirectory(src)
