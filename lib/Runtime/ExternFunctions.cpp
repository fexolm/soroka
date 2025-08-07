#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

#include <cstddef>
#include <cstring>
#include <string>

#include "soroka/Runtime/FunctionRegistry.hpp"
#include "soroka/Runtime/ModuleRegistry.hpp"

extern "C" void sorokaRegisterModule(const char *Name, const char *byteCodeIr,
                                     size_t size) {
  llvm::outs() << "Registering module: " << Name << ", size: " << size
               << " bytes" << "\n";

  soroka::ModuleRegistry::get().registerModule(Name, byteCodeIr, size);

  llvm::outs() << "Module registered successfully.\n";

  llvm::outs() << "Registered modules:\n";
  for (const auto &[moduleName, moduleIR] : soroka::ModuleRegistry::get()) {
    llvm::outs() << "Module Name: " << moduleName << "\n";
  }
  llvm::outs() << "End of registered modules.\n";
}

extern "C" void sorokaRegisterFunction(void *FunctionPtr,
                                       const char *FunctionId,
                                       const char *ModuleId) {
  llvm::outs() << "Registering function: " << FunctionId
               << ", module: " << ModuleId << ", ptr: " << FunctionPtr << "\n";

  soroka::FunctionRegistry::get().registerFunction(FunctionPtr, FunctionId,
                                                   ModuleId);

  std::string ModuleName =
      soroka::FunctionRegistry::get().getModuleName(FunctionId);
  if (!ModuleName.empty()) {
    llvm::outs() << "Function " << FunctionId
                 << " is registered in module: " << ModuleName << "\n";
  } else {
    llvm::outs() << "Function " << FunctionId
                 << " is not registered in any module.\n";
  }

  llvm::Module *moduleIR =
      soroka::ModuleRegistry::get().getDeserializedModule(ModuleId);
  if (moduleIR) {
    llvm::outs() << "Module " << ModuleId << " is registered\n";
  } else {
    llvm::outs() << "Module " << ModuleId << " is not registered.\n";
  }
}
