//===- LLVMPrintFunctionNames.cpp
//---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Example clang plugin which simply prints the names of all the functions
// within the generated LLVM code.
//
//===----------------------------------------------------------------------===//

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Sema/Sema.h>
#include <llvm-20/llvm/ADT/APInt.h>
#include <llvm-20/llvm/IR/Constant.h>
#include <llvm-20/llvm/IR/GlobalVariable.h>
#include <llvm-20/llvm/IR/IRBuilder.h>
#include <llvm-20/llvm/IR/Instructions.h>
#include <llvm/IR/Analysis.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Type.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>
#include <memory>
#include <string>
#include <vector>

using namespace clang;

namespace soroka {

class PrintPass final : public llvm::AnalysisInfoMixin<PrintPass> {
  friend struct llvm::AnalysisInfoMixin<PrintPass>;

public:
  using Result = llvm::PreservedAnalyses;

  Result run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM) {
    llvm::IntegerType *T = llvm::IntegerType::get(M.getContext(), 32);

    llvm::Constant *V =
        llvm::Constant::getIntegerValue(T, llvm::APInt(32, 100500));

    llvm::GlobalVariable *GV = M.getGlobalVariable("kek");
    GV->setInitializer(V);

    M.dump();

    return llvm::PreservedAnalyses::none();
  }
  static bool isRequired() { return true; }
};

void PrintCallback(llvm::PassBuilder &PB) {
  PB.registerPipelineStartEPCallback(
      [](llvm::ModulePassManager &MPM, llvm::OptimizationLevel) {
        MPM.addPass(PrintPass());
      });
}

class EmbedIrASTConsumer : public ASTConsumer {
public:
  EmbedIrASTConsumer(CompilerInstance &Instance) : CI(Instance) {
    CI.getCodeGenOpts().PassBuilderCallbacks.push_back(PrintCallback);
  }

private:
  clang::CompilerInstance &CI;
};

class EmbedIrAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<EmbedIrASTConsumer>(CI);
  }
  bool ParseArgs(const CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }
  PluginASTAction::ActionType getActionType() override {
    return AddBeforeMainAction;
  }
};

} // namespace soroka

static const FrontendPluginRegistry::Add<EmbedIrAction>
    X("llvm-print-fns", "print function names, llvm level");
