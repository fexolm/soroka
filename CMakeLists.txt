cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy;--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy")

project(soroka)

find_package(Clang 20.1 REQUIRED)
find_package(LLVM 20.1 REQUIRED)

add_subdirectory(lib)
add_subdirectory(tests)
# add_subdirectory(examples)