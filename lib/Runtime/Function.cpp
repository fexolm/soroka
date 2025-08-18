#include "soroka/Runtime/Function.hpp"

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/IR/Verifier.h>

#include <memory>
#include <stdexcept>
#include <utility>

#include "soroka/Runtime/FunctionRegistry.hpp"
#include "soroka/Runtime/ModuleRegistry.hpp"

#include "Utils.hpp" // For EmitHelloSoroka

namespace soroka {
Function::Function(void *Address, std::unique_ptr<llvm::orc::LLJIT> jit)
    : Address(Address), jit(std::move(jit)) {}

std::unique_ptr<Function> Function::Compile(FunctionPtr Address) {
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
  if (llvm::verifyModule(*M, &llvm::errs())) {
    llvm::errs() << "Module isn't correct\n";
    throw std::runtime_error("Failed to create LLJIT");
  }
  for (auto &F : *M) {
    for (auto &BB : F) {
      if (!BB.getTerminator()) {
        llvm::errs() << "Basic block without terminator in function: " << F.getName()
                     << "\n";
      }
    }
  }
  llvm::ExitOnError ExitOnErr;
  std::unique_ptr<llvm::orc::LLJIT> jit =
      ExitOnErr(llvm::orc::LLJITBuilder().create());

  soroka::utils::EmitHelloSoroka(M->getFunction(funcEntry.functionId));

  llvm::orc::ThreadSafeModule TSM(std::move(M), std::move(Ctx));
  if (auto Err = jit->addIRModule(std::move(TSM))) {
    llvm::logAllUnhandledErrors(std::move(Err), llvm::errs(),
                                "Adding IR module: ");
    throw std::runtime_error("Failed to add IR module to JIT");
  }

  llvm::errs() << "Function `" << funcEntry.functionId
               << "` compiled and added to JIT.\n";
  auto symbol = ExitOnErr(jit->lookup(funcEntry.functionId));

  return std::make_unique<Function>((FunctionPtr)symbol.getValue(),
                                    std::move(jit));
}

void Function::operator()() const {
  llvm::errs() << "Call function by adress: " << Address << "\n";
  if (Address) {
    FunctionType func = (FunctionType)Address;
    func();
  } else {
    throw std::runtime_error("Function address is null");
  }
}
} // namespace soroka