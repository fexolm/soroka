#pragma once

#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace soroka {
namespace utils {
llvm::Function *CreateGlobalCtor(llvm::LLVMContext &C, llvm::Module &M);
llvm::SmallVector<char, 0> ModuleToObject(llvm::Module &M);
} // namespace utils
} // namespace soroka
