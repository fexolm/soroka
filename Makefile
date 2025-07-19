build_and_run_example:
	clang++ -fplugin=build/lib/Plugin/libsoroka_plugin.so examples/main.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -Lbuild/lib/Runtime -lsoroka_runtime -o example.out
	./example.out
