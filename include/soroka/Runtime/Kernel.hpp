#pragma once

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <memory>

namespace soroka {

class Kernel {
  using FunctionType = void(*)();
  void *Address;
  std::unique_ptr<llvm::orc::LLJIT> jit;

public:
  Kernel(void *Address, std::unique_ptr<llvm::orc::LLJIT> jit);

  static Kernel* Compile(void *Address);

  void operator()() const;
};

} // namespace soroka