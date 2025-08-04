#include <cstdio>

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
  return 0;
}
