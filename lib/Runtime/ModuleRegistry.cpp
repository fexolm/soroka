#include "soroka/Runtime/ModuleRegistry.hpp"

#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/Error.h>

#include <cstddef>
#include <cstdio>
#include <string>

namespace soroka {
ModuleRegistry::ModuleRegistry() {}

ModuleRegistry &ModuleRegistry::get() {
  static ModuleRegistry MR;
  return MR;
}

const std::string &
ModuleRegistry::getDeserializedModule(const std::string &ModuleName) {
  auto it = DeserializedModuleByName.find(ModuleName);
  static const std::string empty_string;
  return it != DeserializedModuleByName.end() ? it->second : empty_string;
}

void ModuleRegistry::registerModule(const std::string &ModuleName,
                                    const char *SerializedModule, size_t size) {
  DeserializedModuleByName[ModuleName] =
      deserializeIRFromBitcode(SerializedModule, size);
}

std::string ModuleRegistry::deserializeIRFromBitcode(const char *ModuleIR,
                                                     size_t size) {
  static const std::string empty_string;
  auto buffer =
      llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(ModuleIR, size));

  if (!buffer) {
    llvm::errs() << "Failed to create memory buffer: \n";
    return std::string();
  }

  llvm::LLVMContext context;
  auto module = llvm::parseBitcodeFile(buffer->getMemBufferRef(), context);
  if (!module) {
    llvm::errs() << "Failed to parse bitcode: "
                 << llvm::toString(module.takeError()) << "\n";
    return std::string();
  }

  std::string ir;
  llvm::raw_string_ostream os(ir);
  module.get()->print(os, nullptr);
  os.flush();
  return ir;
}

} // namespace soroka
