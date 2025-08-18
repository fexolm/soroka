build_and_run_example:
	clang++ -rdynamic -fplugin=build/lib/Plugin/libsoroka_plugin.so examples/main.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core orcjit native` -Iinclude -Lbuild/lib/Runtime -lsoroka_runtime -o example.out -std=c++17
	./example.out
