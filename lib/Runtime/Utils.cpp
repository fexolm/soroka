#include "Utils.hpp"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h>

namespace soroka::utils {

void EmitHelloSoroka(llvm::Function *Func) {
  llvm::Module *M = Func->getParent();
  llvm::BasicBlock &entry = Func->getEntryBlock();
  llvm::IRBuilder<> Builder(&entry.front());
  llvm::FunctionCallee printfFunc = M->getOrInsertFunction(
      "printf",
      llvm::FunctionType::get(Builder.getInt32Ty(), Builder.getPtrTy(), true));
  llvm::Value *helloStr = M->getGlobalVariable("soroka.hello");
  if (!helloStr) {
    helloStr =
        Builder.CreateGlobalString("Hello from Soroka\n", "soroka.hello");
  }
  Builder.CreateCall(printfFunc, {helloStr});
}

} // namespace soroka::utils
