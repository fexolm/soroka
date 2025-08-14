#include "soroka/Runtime/ModuleRegistry.hpp"

#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/Error.h>

#include <cstddef>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace soroka {
ModuleRegistry::ModuleRegistry() {}

ModuleRegistry &ModuleRegistry::get() {
  static ModuleRegistry MR;
  return MR;
}

ModuleContextPair
ModuleRegistry::getDeserializedModule(std::string_view ModuleName) {
  auto it = DeserializedModuleByName.find(ModuleName.data());
  if (it == DeserializedModuleByName.end()) {
    throw std::runtime_error("Module not found: " + std::string(ModuleName) +
                             "\n");
  }

  const ModuleEntry &entry = it->second;
  std::unique_ptr<llvm::LLVMContext> context(new llvm::LLVMContext());
  std::unique_ptr<llvm::Module> module =
      deserializeIRFromBitcode(entry.serializedData, entry.size, *context);
  return ModuleContextPair{
      std::move(module),
      std::move(context),
  };
}

void ModuleRegistry::registerModule(const char *ModuleName,
                                    const char *SerializedModule, size_t size) {
  DeserializedModuleByName[ModuleName] = ModuleEntry{SerializedModule, size};
}

std::unique_ptr<llvm::Module>
ModuleRegistry::deserializeIRFromBitcode(const char *ModuleIR, size_t size,
                                         llvm::LLVMContext &context) {
  std::unique_ptr<llvm::MemoryBuffer> buffer =
      llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(ModuleIR, size));

  if (!buffer) {
    throw std::runtime_error("Failed to create memory buffer\n");
  }

  llvm::Expected<std::unique_ptr<llvm::Module>> module =
      llvm::parseBitcodeFile(buffer->getMemBufferRef(), context);
  if (llvm::Error err = module.takeError()) {
    throw std::runtime_error(
        "Failed to parse bitcode: " + llvm::toString(std::move(err)) + "\n");
  }

  return std::move(module.get());
}

} // namespace soroka
