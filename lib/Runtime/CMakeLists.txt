add_library(soroka_runtime STATIC ModuleRegistry.cpp)
target_compile_options(soroka_runtime PRIVATE -Wall -Wpedantic -Werror)
target_include_directories(soroka_runtime PUBLIC ${CMAKE_SOURCE_DIR}/include)
