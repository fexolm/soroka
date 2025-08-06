#ifdef DEBUG
#include "llvm/Support/raw_ostream.h"
#endif

#include <cstddef>
#include <cstring>

#include "soroka/Runtime/FunctionRegistry.hpp"
#include "soroka/Runtime/ModuleRegistry.hpp"

extern "C" void sorokaRegisterModule(const char *Name, const char *byteCodeIr,
                                     size_t size) {
#ifdef DEBUG
  llvm::outs() << "Registering module: " << Name << ", size: " << size
               << " bytes" << "\n";
#endif

  soroka::ModuleRegistry::get().registerModule(Name, byteCodeIr, size);

#ifdef DEBUG
  llvm::outs() << "Module registered successfully.\n";

  llvm::outs() << "Registered modules:\n";
  for (const auto &[moduleName, moduleIR] : soroka::ModuleRegistry::get()) {
    llvm::outs() << "Module Name: " << moduleName << "\n";
    llvm::outs() << "Module IR: " << moduleIR << "\n";
    llvm::outs() << "--------------------------\n";
  }
  llvm::outs() << "End of registered modules.\n";
#endif
}

extern "C" void sorokaRegisterFunction(void *FunctionPtr,
                                       const char *FunctionId,
                                       const char *ModuleId) {
#ifdef DEBUG
  llvm::outs() << "Registering function: " << FunctionId
               << ", module: " << ModuleId << ", ptr: " << FunctionPtr << "\n";
#endif

  soroka::FunctionRegistry::get().registerFunction(FunctionPtr, FunctionId,
                                                   ModuleId);

#ifdef DEBUG
  const std::string ModuleName =
      soroka::FunctionRegistry::get().getModuleName(FunctionId);
  if (!ModuleName.empty()) {
    llvm::outs() << "Function " << FunctionId
                 << " is registered in module: " << ModuleName << "\n";
  } else {
    llvm::outs() << "Function " << FunctionId
                 << " is not registered in any module.\n";
  }

  std::string moduleIR =
      soroka::ModuleRegistry::get().getDeserializedModule(ModuleId);
  if (!moduleIR.empty()) {
    llvm::outs() << "Module " << ModuleId << " is registered\n";
  } else {
    llvm::outs() << "Module " << ModuleId << " is not registered.\n";
  }
#endif
}
