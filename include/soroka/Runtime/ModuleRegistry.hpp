#pragma once
#include <llvm/IR/Module.h>

#include <memory>
#include <string_view>
#include <unordered_map>

namespace soroka {

class ModuleRegistry {
public:
  using Container =
      std::unordered_map<std::string, std::unique_ptr<llvm::Module>>;
  using Iterator = typename Container::iterator;
  using ConstIterator = typename Container::const_iterator;

  Iterator begin() { return DeserializedModuleByName.begin(); }
  Iterator end() { return DeserializedModuleByName.end(); }
  ConstIterator begin() const { return DeserializedModuleByName.begin(); }
  ConstIterator end() const { return DeserializedModuleByName.end(); }

  static ModuleRegistry &get();

  llvm::Module *getDeserializedModule(std::string_view ModuleName);

  void registerModule(const char *ModuleName, const char *SerializedModule,
                      size_t size);

private:
  ModuleRegistry();

  std::unique_ptr<llvm::Module> deserializeIRFromBitcode(const char *ModuleIR,
                                                         size_t size);

  std::unordered_map<std::string, std::unique_ptr<llvm::Module>>
      DeserializedModuleByName;
};
} // namespace soroka
