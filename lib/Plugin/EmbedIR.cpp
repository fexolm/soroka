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
#include <clang/CodeGen/BackendUtil.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/CodeGen/ModuleBuilder.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Sema/Sema.h>
// #include <cstddef> // for size_t
#include <cstdint>
#include <llvm-20/llvm/ADT/APInt.h>
#include <llvm-20/llvm/IR/Constant.h>
#include <llvm-20/llvm/IR/Constants.h>
#include <llvm-20/llvm/IR/GlobalVariable.h>
#include <llvm-20/llvm/IR/IRBuilder.h>
#include <llvm-20/llvm/IR/Instructions.h>
// #include <llvm-20/llvm/Support/MemoryBuffer.h>
// #include <llvm-20/llvm/Support/MemoryBufferRef.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Analysis.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Type.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <string>
#include <vector>

namespace soroka {

class EmbedIRPass final : public llvm::AnalysisInfoMixin<EmbedIRPass> {
  friend struct llvm::AnalysisInfoMixin<EmbedIRPass>;

public:
  using Result = llvm::PreservedAnalyses;

  Result run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM) {
    std::string Data;
    llvm::raw_string_ostream OS(Data);
    llvm::WriteBitcodeToFile(M, OS, /* ShouldPreserveUseListOrder */ true);
    llvm::ArrayRef<uint8_t> ModuleData = llvm::ArrayRef<uint8_t>(
        (const uint8_t *)OS.str().data(), OS.str().size());
    llvm::Constant *V = llvm::Constant::getIntegerValue(
        llvm::IntegerType::get(M.getContext(), 32),
        llvm::APInt(32, ModuleData.size()));
    llvm::GlobalVariable *GV = M.getGlobalVariable("kek");
    GV->setInitializer(V);
    llvm::outs() << "Kek: " << ModuleData.size() << " bytes\n";

    llvm::GlobalVariable *GV2 = M.getGlobalVariable("kek_bytes");
    llvm::Constant *ModuleConstant =
        llvm::ConstantDataArray::get(M.getContext(), ModuleData);
    GV2->setInitializer(ModuleConstant);
    return llvm::PreservedAnalyses::none();
  }
  static bool isRequired() { return true; }
};

void PrintCallback(llvm::PassBuilder &PB) {
  PB.registerPipelineStartEPCallback(
      [](llvm::ModulePassManager &MPM, llvm::OptimizationLevel) {
        MPM.addPass(EmbedIRPass());
      });
}

class EmbedIrASTConsumer : public clang::ASTConsumer {
public:
  EmbedIrASTConsumer(clang::CompilerInstance &Instance) : CI(Instance) {
    CI.getCodeGenOpts().PassBuilderCallbacks.push_back(PrintCallback);
  }

  EmbedIrASTConsumer(const EmbedIrASTConsumer &) = delete;

private:
  clang::CompilerInstance &CI;
};

class EmbedIrAction : public clang::PluginASTAction {
protected:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef) override {
    return std::make_unique<EmbedIrASTConsumer>(CI);
  }

  bool ParseArgs(const clang::CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }
  PluginASTAction::ActionType getActionType() override {
    return AddBeforeMainAction;
  }
};

} // namespace soroka

static const clang::FrontendPluginRegistry::Add<soroka::EmbedIrAction>
    X(/*Name=*/"embed-ir",
      /*Description=*/"Embed jitted functions IR to binary");
