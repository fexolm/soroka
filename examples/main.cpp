#include <cstdio>

#include <soroka/Soroka.hpp>

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
  printf("Call before JIT compiled function\n");
  sorokaFunction();
  auto func = soroka::compile(sorokaFunction);
  if (!func) {
    fprintf(stderr, "Failed to compile function\n");
    return 1;
  }
  (*func)(); // Call the JIT compiled function again
  return 0;
}
