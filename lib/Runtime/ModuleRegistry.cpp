#include "soroka/Runtime/ModuleRegistry.h"

#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include <cstddef>
#include <llvm/ADT/StringRef.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/Error.h>

namespace soroka {
ModuleRegistry::ModuleRegistry() {}

ModuleRegistry &ModuleRegistry::get() {
  static ModuleRegistry MR;
  return MR;
}

const char *ModuleRegistry::getFunctionName(void *Ptr) {
  auto it = NameByFuncPtr.find(Ptr);
  return it != NameByFuncPtr.end() ? it->second : nullptr;
}

void ModuleRegistry::registerFunction(void *Ptr, const char *Name) {
  NameByFuncPtr[Ptr] = Name;
}

void printIRFromBitcode(const char *ModuleIR, size_t size) {
  auto buffer =
      llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(ModuleIR, size));
  if (!buffer) {
    llvm::errs() << "Failed to create memory buffer: \n";
    return;
  }
  llvm::LLVMContext context;
  auto module = llvm::parseBitcodeFile(buffer->getMemBufferRef(), context);
  if (!module) {
    llvm::errs() << "Failed to parse bitcode: "
                 << llvm::toString(module.takeError()) << "\n";
    return;
  }
  (*module)->print(llvm::outs(), nullptr);
}

extern "C" void soroka_register_module(const char *Name, const char *byteCodeIr,
                                       size_t size) {
  llvm::outs() << "Registering module: " << Name << ", size: " << size
               << " bytes" << "\n";
  printIRFromBitcode(byteCodeIr, size);
}

} // namespace soroka
