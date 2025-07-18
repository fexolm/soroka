#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

namespace soroka {
namespace runtime_functions {
void EmitRegisterModuleCall(llvm::Module &M, llvm::IRBuilder<> &Builder,
                            llvm::ArrayRef<llvm::Value *> Args);
} // namespace runtime_functions
} // namespace soroka
