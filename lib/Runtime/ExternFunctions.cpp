#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/Error.h>

#include <cstddef>
#include <cstring>

#include "soroka/Runtime/FunctionRegistry.hpp"
#include "soroka/Runtime/ModuleRegistry.hpp"

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

extern "C" void sorokaRegisterModule(const char *Name, const char *byteCodeIr,
                                     size_t size) {
  llvm::outs() << "Registering module: " << Name << ", size: " << size
               << " bytes" << "\n";
  // printIRFromBitcode(byteCodeIr, size);
  // soroka::ModuleRegistry &ModuleRegistry = soroka::ModuleRegistry::get();
  soroka::ModuleRegistry::get().registerModule(Name, byteCodeIr, size);
  llvm::outs() << "Module registered successfully.\n";

  // // Optionally, print all registered modules
  // llvm::outs() << "Registered modules:\n";
  // for (const auto &[module_name, module_entry] :
  //      soroka::ModuleRegistry::get()) {
  //   llvm::outs() << "Module Name: " << module_name << "\n";
  //   printIRFromBitcode(module_entry.serializedData, module_entry.size);
  //   llvm::outs() << "--------------------------\n";
  // }
  // llvm::outs() << "End of registered modules.\n";
}

extern "C" void sorokaRegisterFunction(const char *FunctionId, const char *ModuleId) {
  llvm::outs() << "Registering function: " << FunctionId << ", module: " << ModuleId
               << "\n";
  soroka::FunctionRegistry &FunctionRegistry = soroka::FunctionRegistry::get();
  FunctionRegistry.registerFunction(FunctionId, ModuleId);

  // Optionally, print module name for the function
  const std::string ModuleName = FunctionRegistry.getModuleName(FunctionId);
  if (!ModuleName.empty()) {
    llvm::outs() << "Function " << FunctionId
                 << " is registered in module: " << ModuleName << "\n";
  } else {
    llvm::outs() << "Function " << FunctionId << " is not registered in any module.\n";
  }

  // Optionally, check if the module is registered
  auto moduleEntry = soroka::ModuleRegistry::get().getSerializedModule(ModuleId);
  if (!moduleEntry.first.empty()) {
    llvm::outs() << "Module " << moduleEntry.first
                 << " is registered with size: " << moduleEntry.second.size
                 << " bytes.\n";
  } else {
    llvm::outs() << "Module " << ModuleId << " is not registered.\n";
  }
}
