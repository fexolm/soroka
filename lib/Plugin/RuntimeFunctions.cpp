#include "soroka/Plugin/RuntimeFunctions.hpp"

#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/BasicBlock.h>

#include "soroka/Plugin/Utils.hpp"

namespace soroka {
namespace runtime_functions {

void EmitRegisterModuleCall(llvm::Module &M, llvm::ArrayRef<llvm::Value *> Args) {
  llvm::StringRef Name = "soroka_register_module";
  if (M.getFunction(Name) != nullptr) {
    llvm::errs() << "Function " << Name << " already exists in the module "
                 << M.getName() << ".\n";
    return; // Function already exists, no need to emit again.
  }

  llvm::LLVMContext &C = M.getContext();
  llvm::Type *VoidTy = llvm::Type::getVoidTy(C);
  llvm::PointerType *VoidPtrTy = llvm::PointerType::get(C, 0);
  llvm::PointerType *CharPtrTy =
      llvm::PointerType::get(llvm::Type::getInt8Ty(C), 0);
  llvm::IntegerType *IntTy = llvm::IntegerType::getInt64Ty(C);
  llvm::FunctionType *FTy =
      llvm::FunctionType::get(VoidTy, {VoidPtrTy, CharPtrTy, IntTy}, false);
  llvm::Function *RegisterModuleF = llvm::Function::Create(
      FTy, llvm::Function::ExternalLinkage, Name, &M);

  // Create ctor function to register the module
  llvm::Function *FSorokaGlobalCtor = utils::CreateGlobalCtor(C, M);

  // Fill the function body
  llvm::BasicBlock *BB =
      llvm::BasicBlock::Create(C, "entry", FSorokaGlobalCtor);
  llvm::IRBuilder<> Builder(BB);
  Builder.CreateCall(RegisterModuleF, Args);
  Builder.CreateRetVoid();
}

} // namespace runtime_functions
} // namespace soroka
