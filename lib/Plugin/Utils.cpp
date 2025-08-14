#include "Utils.hpp"

#include <llvm/ADT/SmallVector.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils/ModuleUtils.h>

namespace soroka {
namespace utils {

llvm::Function *CreateGlobalCtor(llvm::LLVMContext &C, llvm::Module &M) {
  llvm::Type *Void = llvm::Type::getVoidTy(C);
  llvm::FunctionType *FTy = llvm::FunctionType::get(Void, false);
  llvm::Function *FGlobalCtor = llvm::Function::Create(
      FTy, llvm::Function::InternalLinkage, "soroka_global_ctor", &M);
  llvm::appendToGlobalCtors(M, FGlobalCtor, 0);
  return FGlobalCtor;
}

llvm::SmallVector<char, 0> SerializeModule(llvm::Module &M) {
  llvm::SmallVector<char, 0> binaryData;
  llvm::raw_svector_ostream OS(binaryData);
  llvm::WriteBitcodeToFile(M, OS, /* ShouldPreserveUseListOrder */ true);
  return binaryData;
}

llvm::Constant *EmbedBinaryData(llvm::Module &M,
                                llvm::SmallVector<char, 0> &Data) {
  llvm::Constant *DataConstant =
      llvm::ConstantDataArray::get(M.getContext(), Data);
  llvm::GlobalVariable *GV = new llvm::GlobalVariable(
      M, DataConstant->getType(),
      true, // isConstant
      llvm::GlobalValue::PrivateLinkage, DataConstant, "soroka.module_data");
  return GV;
}

void EmitRegisterModuleCall(llvm::Module &M, llvm::IRBuilder<> &Builder,
                            llvm::ArrayRef<llvm::Value *> Args) {
  llvm::StringRef Name = "sorokaRegisterModule";
  llvm::Function *RegisterModuleFunc = M.getFunction(Name);

  // If the function does not exist, create it
  if (RegisterModuleFunc == nullptr) {
    llvm::LLVMContext &C = M.getContext();
    llvm::Type *VoidTy = llvm::Type::getVoidTy(C);
    llvm::PointerType *PtrTy = llvm::PointerType::getUnqual(C);
    llvm::IntegerType *IntTy = llvm::IntegerType::getInt64Ty(C);
    llvm::FunctionType *FTy =
        llvm::FunctionType::get(VoidTy, {PtrTy, PtrTy, IntTy}, false);
    RegisterModuleFunc =
        llvm::Function::Create(FTy, llvm::Function::ExternalLinkage, Name, &M);
  }

  Builder.CreateCall(RegisterModuleFunc, Args);
}

void EmitRegisterFunctionCall(llvm::Module &M, llvm::IRBuilder<> &Builder,
                              llvm::ArrayRef<llvm::Value *> Args) {
  llvm::StringRef Name = "sorokaRegisterFunction";
  llvm::Function *RegisterFunctionFunc = M.getFunction(Name);

  // If the function does not exist, create it
  if (RegisterFunctionFunc == nullptr) {
    llvm::LLVMContext &C = M.getContext();
    llvm::Type *VoidTy = llvm::Type::getVoidTy(C);
    llvm::PointerType *PtrTy = llvm::PointerType::getUnqual(C);
    llvm::FunctionType *FTy =
        llvm::FunctionType::get(VoidTy, {PtrTy, PtrTy, PtrTy}, false);
    RegisterFunctionFunc =
        llvm::Function::Create(FTy, llvm::Function::ExternalLinkage, Name, &M);
  }

  Builder.CreateCall(RegisterFunctionFunc, Args);
}

} // namespace utils
} // namespace soroka
