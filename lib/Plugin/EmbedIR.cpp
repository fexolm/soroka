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
#include <cstdint>
#include <llvm-20/llvm/ADT/APInt.h>
#include <llvm-20/llvm/IR/Constant.h>
#include <llvm-20/llvm/IR/Constants.h>
#include <llvm-20/llvm/IR/GlobalVariable.h>
#include <llvm-20/llvm/IR/IRBuilder.h>
#include <llvm-20/llvm/IR/Instructions.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Analysis.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Type.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils/ModuleUtils.h>
#include <memory>
#include <string>
#include <vector>

namespace soroka {

class EmbedIRPass final : public llvm::AnalysisInfoMixin<EmbedIRPass> {
  friend struct llvm::AnalysisInfoMixin<EmbedIRPass>;

public:
  using Result = llvm::PreservedAnalyses;

  Result run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM) {

    // Create ctor function to register the module
    llvm::LLVMContext &C = M.getContext();
    llvm::Type *Void = llvm::Type::getVoidTy(C);
    llvm::FunctionType *FTy = llvm::FunctionType::get(Void, false);
    llvm::Function *FGlobalCtor = llvm::Function::Create(
        FTy, llvm::Function::InternalLinkage, "soroka_global_ctor", &M);
    llvm::appendToGlobalCtors(M, FGlobalCtor, 0);

    // Fill the function body
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(C, "entry", FGlobalCtor);
    llvm::IRBuilder<> Builder(BB);

    llvm::FunctionCallee RegisterModuleF =
        M.getFunction("soroka_register_module");

    if (!RegisterModuleF) {
      llvm::errs()
          << "Function 'soroka_register_module' not found in module.\n";
      return llvm::PreservedAnalyses::none();
    }

    // Create call to register the module
    std::string Data;
    llvm::raw_string_ostream OS(Data);
    llvm::WriteBitcodeToFile(M, OS, /* ShouldPreserveUseListOrder */ true);
    llvm::ArrayRef<uint8_t> ModuleData = llvm::ArrayRef<uint8_t>(
        (const uint8_t *)OS.str().data(), OS.str().size());
    llvm::Constant *ModuleConstant =
        llvm::ConstantDataArray::get(M.getContext(), ModuleData);
    llvm::outs() << "ModuleData.size(): " << ModuleData.size() << " bytes\n";

    llvm::Constant *ModuleNameConstant =
        llvm::ConstantDataArray::getString(C, M.getName().str(), true);

    Builder.CreateCall(RegisterModuleF, {ModuleConstant, ModuleNameConstant});
    Builder.CreateRetVoid();

    M.dump();
    return llvm::PreservedAnalyses::none();
  }
  static bool isRequired() { return true; }
};

class RegisterModulePass final
    : public llvm::AnalysisInfoMixin<RegisterModulePass> {
  friend struct llvm::AnalysisInfoMixin<RegisterModulePass>;

public:
  using Result = llvm::PreservedAnalyses;

  Result run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM) {
    llvm::StringRef Name = "soroka_register_module";
    if (M.getFunction(Name) != nullptr) {
      return llvm::PreservedAnalyses::none();
    }

    llvm::LLVMContext &C = M.getContext();
    llvm::Type *Void = llvm::Type::getVoidTy(C);
    llvm::PointerType *VoidPtr = llvm::PointerType::get(C, 0);
    llvm::PointerType *CharPtr =
        llvm::PointerType::get(llvm::Type::getInt8Ty(C), 0);
    llvm::FunctionType *FTy =
        llvm::FunctionType::get(Void, {VoidPtr, CharPtr}, false);
    llvm::Function::Create(FTy, llvm::Function::ExternalLinkage, Name, &M);
    return llvm::PreservedAnalyses::none();
  }

  static bool isRequired() { return true; }
};

void PrintCallback(llvm::PassBuilder &PB) {
  PB.registerPipelineStartEPCallback(
      [](llvm::ModulePassManager &MPM, llvm::OptimizationLevel) {
        MPM.addPass(RegisterModulePass());
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
