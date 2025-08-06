#pragma once
#include <string>
#include <unordered_map>
#include <utility>

namespace soroka {

class ModuleRegistry {
public:

#ifdef DEBUG
  using Container = std::unordered_map<std::string, std::string>;
  using Iterator = typename Container::iterator;
  using ConstIterator = typename Container::const_iterator;

  Iterator begin() { return DeserializedModuleByName.begin(); }
  Iterator end() { return DeserializedModuleByName.end(); }
  ConstIterator begin() const { return DeserializedModuleByName.begin(); }
  ConstIterator end() const { return DeserializedModuleByName.end(); }
#endif

  static ModuleRegistry &get();

  const std::string &getDeserializedModule(const std::string &ModuleName);

  void registerModule(const std::string &ModuleName,
                      const char *SerializedModule, size_t size);

private:
  ModuleRegistry();

  std::string deserializeIRFromBitcode(const char *ModuleIR, size_t size);

  std::unordered_map<std::string, std::string> DeserializedModuleByName;
};
} // namespace soroka
