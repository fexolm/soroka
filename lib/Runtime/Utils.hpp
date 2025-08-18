#pragma once

#include <llvm/IR/Function.h>

namespace soroka::utils {

// Emit a call to printf with a "Hello from Soroka" message for debugging
void EmitHelloSoroka(llvm::Function *Func);

} // namespace soroka::utils
