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

#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Analysis.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <memory>
#include <string>
#include <vector>

#include "soroka/Plugin/RuntimeFunctions.hpp"
#include "soroka/Plugin/Utils.hpp"

namespace soroka {

class EmbedIRPass final : public llvm::AnalysisInfoMixin<EmbedIRPass> {
  friend struct llvm::AnalysisInfoMixin<EmbedIRPass>;

public:
  using Result = llvm::PreservedAnalyses;

  Result run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM) {
    llvm::LLVMContext &C = M.getContext();

    // Create call to register the module
    llvm::SmallVector<char, 0> ModuleData = utils::ModuleToObject(M);
    llvm::Constant *ModuleConstant =
        llvm::ConstantDataArray::get(M.getContext(), ModuleData);
    llvm::GlobalVariable *ModuleGV =
        new llvm::GlobalVariable(M, ModuleConstant->getType(),
                                 true, // isConstant
                                 llvm::GlobalValue::PrivateLinkage,
                                 ModuleConstant, "soroka.module_data");

    llvm::Constant *ModuleNameConstant =
        llvm::ConstantDataArray::getString(C, M.getName().str(), true);
    llvm::GlobalVariable *ModuleNameGV =
        new llvm::GlobalVariable(M, ModuleNameConstant->getType(),
                                 true, // isConstant
                                 llvm::GlobalValue::PrivateLinkage,
                                 ModuleNameConstant, "soroka.module_name");

    llvm::Constant *ModuleSizeConstant = llvm::ConstantInt::get(
        llvm::Type::getInt64Ty(M.getContext()), ModuleData.size());

    runtime_functions::EmitRegisterModuleCall(
        M, {ModuleNameGV, ModuleGV, ModuleSizeConstant});

    if (llvm::verifyModule(M, &(llvm::errs()))) {
      llvm::errs() << "Module verification failed\n";
      return llvm::PreservedAnalyses::none();
    }

    M.dump();
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
