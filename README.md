```
clang++ -fplugin=build/lib/Plugin/libsoroka_plugin.so -Iinclude examples/main.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -Lbuild/lib/Runtime -lsoroka_runtime
```
