#pragma once

#include "soroka/Runtime/Function.hpp"

namespace soroka {
template <typename F, typename... A> auto compile(F &&Func, A &&...Args) {
  return soroka::Function::Compile(reinterpret_cast<void *>(Func));
}
} // namespace soroka