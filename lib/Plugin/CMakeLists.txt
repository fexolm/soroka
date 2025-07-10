set(WHOLE_ARCHIVE_FLAG "-Wl,--whole-archive")
set(NO_WHOLE_ARCHIVE_FLAG "-Wl,--no-whole-archive")

add_library(soroka_plugin SHARED EmbedIR.cpp)
target_include_directories(soroka_plugin PRIVATE ${CLANG_INCLUDE_DIRS})
target_compile_options(soroka_plugin PRIVATE -Wall -Wpedantic -Werror)
target_link_libraries(soroka_plugin PRIVATE ${WHOLE_ARCHIVE_FLAG} soroka_runtime ${NO_WHOLE_ARCHIVE_FLAG})

add_library(soroka_plugin_flags INTERFACE)
add_dependencies(soroka_plugin_flags soroka_plugin)
target_compile_options(soroka_plugin_flags INTERFACE "-fplugin=${CMAKE_CURRENT_BINARY_DIR}/libsoroka_plugin.so")


