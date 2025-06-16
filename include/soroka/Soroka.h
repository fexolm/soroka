#pragma once

namespace soroka {
template <typename F, typename... A> auto compile(F &&Func, A &&...Args) {}
} // namespace soroka