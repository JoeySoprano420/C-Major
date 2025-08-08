cmake_minimum_required(VERSION 3.10)
project(CMajorCompiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(cmajor
    main.cpp
    Lexer.cpp
    Parser.cpp
)
