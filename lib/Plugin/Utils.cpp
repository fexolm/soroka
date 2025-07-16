#include "soroka/Plugin/Utils.hpp"

#include <llvm/ADT/SmallVector.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/DerivedTypes.h>
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

llvm::SmallVector<char, 0> ModuleToObject(llvm::Module &M) {
  llvm::SmallVector<char, 0> binaryData;
  llvm::raw_svector_ostream OS(binaryData);
  llvm::WriteBitcodeToFile(M, OS, /* ShouldPreserveUseListOrder */ true);
  return binaryData;
}

} // namespace utils
} // namespace soroka
