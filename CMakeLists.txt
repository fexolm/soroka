cmake_minimum_required(VERSION 3.15)

project(soroka)

find_package(Clang 20.1 REQUIRED)
find_package(LLVM 20.1 REQUIRED)

add_subdirectory(lib)
add_subdirectory(tests)
add_subdirectory(examples)