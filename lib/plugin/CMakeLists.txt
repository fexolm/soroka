add_library(soroka_plugin SHARED print_functions.cpp)

target_include_directories(soroka_plugin PRIVATE ${CLANG_INCLUDE_DIRS})