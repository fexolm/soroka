#pragma once

#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace soroka {
namespace utils {
llvm::Function *CreateGlobalCtor(llvm::LLVMContext &C, llvm::Module &M);
llvm::SmallVector<char, 0> SerializeModule(llvm::Module &M);
llvm::Constant *EmbedBinaryData(llvm::Module &M,
                                llvm::SmallVector<char, 0> &Data);
void EmitRegisterModuleCall(llvm::Module &M, llvm::IRBuilder<> &Builder,
                            llvm::ArrayRef<llvm::Value *> Args);
} // namespace utils
} // namespace soroka
