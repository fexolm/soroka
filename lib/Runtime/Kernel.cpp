#include "soroka/Runtime/Kernel.hpp"

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/IR/Verifier.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "soroka/Runtime/FunctionRegistry.hpp"
#include "soroka/Runtime/ModuleRegistry.hpp"

#include "Utils.hpp" // For EmitHelloSoroka

namespace soroka {
Kernel::Kernel(void *Address, std::unique_ptr<llvm::orc::LLJIT> jit)
    : Address(Address), jit(std::move(jit)) {}

Kernel *Kernel::Compile(void *Address) {
  if (!Address) {
    throw std::runtime_error("Function address is null");
  }
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  soroka::ModuleRegistry &moduleRegistry = soroka::ModuleRegistry::get();
  soroka::FunctionRegistry &functionRegistry = soroka::FunctionRegistry::get();

  soroka::FunctionEntry funcEntry =
      functionRegistry.getFunctionModuleAndIdName(Address);

  llvm::errs() << "Compiling function `" << funcEntry.functionId
               << "` by address: " << Address << "\n";

  auto [M, Ctx] = moduleRegistry.getDeserializedModule(funcEntry.moduleName);

  std::string verifyErrs;
  llvm::raw_string_ostream verifyStream(verifyErrs);
  if (llvm::verifyModule(*M, &verifyStream)) {
    verifyStream.flush();
    llvm::errs() << "Module verification failed:\n" << verifyErrs << "\n";
    throw std::runtime_error("Failed to create LLJIT: " + verifyErrs);
  }

  for (auto &F : *M) {
    for (auto &BB : F) {
      if (!BB.getTerminator()) {
        llvm::errs() << "Basic block without terminator in function: "
                     << F.getName() << "\n";
      }
    }
  }

  llvm::ExitOnError ExitOnErr;
  std::unique_ptr<llvm::orc::LLJIT> jit =
      ExitOnErr(llvm::orc::LLJITBuilder().create());

  llvm::orc::ThreadSafeModule TSM(std::move(M), std::move(Ctx));
  if (auto Err = jit->addIRModule(std::move(TSM))) {
    llvm::logAllUnhandledErrors(std::move(Err), llvm::errs(),
                                "Adding IR module: ");
    throw std::runtime_error("Failed to add IR module to JIT");
  }

  llvm::errs() << "Function `" << funcEntry.functionId
               << "` compiled and added to JIT.\n";
  auto symbol = ExitOnErr(jit->lookup(funcEntry.functionId));
  Kernel *result = new Kernel((void *)symbol.getValue(), std::move(jit));
  return result;
}

void Kernel::operator()() const {
  llvm::errs() << "Call function by adress: " << Address << "\n";
  if (Address) {
    FunctionType func = (FunctionType)Address;
    func();
  } else {
    throw std::runtime_error("Function address is null");
  }
}
} // namespace soroka