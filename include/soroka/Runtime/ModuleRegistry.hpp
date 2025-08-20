#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <memory>
#include <string_view>
#include <unordered_map>

namespace soroka {

struct ModuleContextPair {
  std::unique_ptr<llvm::Module> module;
  std::unique_ptr<llvm::LLVMContext> context;

  ~ModuleContextPair() {
    module.reset();
    context.reset();
  }
};

struct ModuleEntry {
  const char *serializedData = nullptr;
  size_t size = 0;
};

class ModuleRegistry {
public:
  using Container = std::unordered_map<std::string, ModuleEntry>;
  using Iterator = typename Container::iterator;
  using ConstIterator = typename Container::const_iterator;

  Iterator begin() { return DeserializedModuleByName.begin(); }
  Iterator end() { return DeserializedModuleByName.end(); }
  ConstIterator begin() const { return DeserializedModuleByName.begin(); }
  ConstIterator end() const { return DeserializedModuleByName.end(); }

  static ModuleRegistry &get();

  ModuleContextPair getDeserializedModule(std::string_view ModuleName);

  void registerModule(const char *ModuleName, const char *SerializedModule,
                      size_t size);

private:
  ModuleRegistry();

  std::unique_ptr<llvm::Module>
  deserializeIRFromBitcode(const char *ModuleIR, size_t size,
                           llvm::LLVMContext &context);

  std::unordered_map<std::string, ModuleEntry> DeserializedModuleByName;
};
} // namespace soroka
