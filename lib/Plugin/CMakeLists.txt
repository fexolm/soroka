add_library(soroka_plugin SHARED EmbedIR.cpp Utils.cpp)
target_include_directories(soroka_plugin PRIVATE ${CLANG_INCLUDE_DIRS})
target_compile_options(soroka_plugin PRIVATE -Wall -Wpedantic -Werror)

add_library(soroka_plugin_flags INTERFACE)
add_dependencies(soroka_plugin_flags soroka_plugin)
target_compile_options(soroka_plugin_flags INTERFACE "-fplugin=${CMAKE_CURRENT_BINARY_DIR}/libsoroka_plugin.so")
