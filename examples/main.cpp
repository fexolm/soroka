#include <cstdio>

#include "soroka/Runtime/FunctionRegistry.hpp"
#include "soroka/Runtime/ModuleRegistry.hpp"

#define SOROKA_JIT __attribute__((section("soroka")))

SOROKA_JIT void sorokaFunction() {
  printf("%s is a JIT compiled function.\n", __func__);
}

SOROKA_JIT void sorokaFunction1() {
  printf("%s is a JIT compiled function.\n", __func__);
}

SOROKA_JIT void sorokaFunction2() {
  printf("%s is a JIT compiled function.\n", __func__);
}

int main() {
  printf("Hello world\n");
  sorokaFunction();
  sorokaFunction1();
  sorokaFunction2();
  soroka::ModuleRegistry &ModuleRegistry = soroka::ModuleRegistry::get();
  llvm::Module *moduleIR =
      ModuleRegistry.getDeserializedModule("examples/main.cpp");
  if (moduleIR) {
    printf("Module examples/main.cpp is registered\n");
  } else {
    printf("Module examples/main.cpp is not registered.\n");
  }

  soroka::FunctionRegistry &FunctionRegistry = soroka::FunctionRegistry::get();
  void *FuncPtr = FunctionRegistry.getFunctionPtr("_Z15sorokaFunction2v");
  if (FuncPtr) {
    printf("Function pointer for _Z15sorokaFunction2v: %p\n", FuncPtr);
  } else {
    printf(
        "Function pointer for _Z15sorokaFunction2v not found in registry.\n");
  }
  printf("Calling _Z15sorokaFunction2v by ptr\n");
  using FuncPtrType = void (*)();
  FuncPtrType func = (FuncPtrType)FuncPtr;
  func();
  return 0;
}
