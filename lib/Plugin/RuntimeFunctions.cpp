#include "soroka/Plugin/RuntimeFunctions.hpp"

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

namespace soroka {
namespace runtime_functions {

void EmitRegisterModuleCall(llvm::Module &M, llvm::IRBuilder<> &Builder,
                            llvm::ArrayRef<llvm::Value *> Args) {
  llvm::StringRef Name = "soroka_register_module";
  llvm::Function *RegisterModuleF = M.getFunction(Name);

  // If the function does not exist, create it
  if (RegisterModuleF == nullptr) {
    llvm::LLVMContext &C = M.getContext();
    llvm::Type *VoidTy = llvm::Type::getVoidTy(C);
    llvm::PointerType *VoidPtrTy = llvm::PointerType::getUnqual(C);
    llvm::PointerType *CharPtrTy =
        llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(C));
    llvm::IntegerType *IntTy = llvm::IntegerType::getInt64Ty(C);
    llvm::FunctionType *FTy =
        llvm::FunctionType::get(VoidTy, {VoidPtrTy, CharPtrTy, IntTy}, false);
    RegisterModuleF =
        llvm::Function::Create(FTy, llvm::Function::ExternalLinkage, Name, &M);
  }

  // Fill the function body
  Builder.CreateCall(RegisterModuleF, Args);
  Builder.CreateRetVoid();
}

} // namespace runtime_functions
} // namespace soroka
