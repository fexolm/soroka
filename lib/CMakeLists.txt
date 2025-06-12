find_package(Clang 20.1 REQUIRED)
find_package(LLVM 20.1 REQUIRED)

add_subdirectory(plugin)
add_subdirectory(runtime)