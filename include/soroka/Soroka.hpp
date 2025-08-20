#pragma once

#include "Runtime/Kernel.hpp"
#include "soroka/Runtime/Kernel.hpp"

#define SOROKA_JIT extern "C" __attribute__((section("soroka")))

namespace soroka {
template <typename F, typename... A> auto compile(F &&Func, A &&...Args) {
  return soroka::Kernel::Compile(reinterpret_cast<void *>(Func));
}
} // namespace soroka