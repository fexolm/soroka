#pragma once

#include <llvm-20/llvm/ExecutionEngine/Orc/LLJIT.h>
#include <memory>

namespace soroka {

class Function {
  using FunctionPtr = void*;
  using FunctionType = void(*)();
  void *Address;
  std::unique_ptr<llvm::orc::LLJIT> jit;

public:
  Function(void *Address, std::unique_ptr<llvm::orc::LLJIT> jit);

  static std::unique_ptr<Function> Compile(FunctionPtr Address);

  void operator()() const;
};

} // namespace soroka