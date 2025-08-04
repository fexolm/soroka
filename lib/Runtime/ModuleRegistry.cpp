#include "soroka/Runtime/ModuleRegistry.hpp"

#include <cstddef>
#include <cstdio>
#include <string>
#include <utility>

namespace soroka {
ModuleRegistry::ModuleRegistry() {}

ModuleRegistry &ModuleRegistry::get() {
  static ModuleRegistry MR;
  return MR;
}

std::pair<std::string, ModuleEntry>
ModuleRegistry::getSerializedModule(const std::string &ModuleName) {
  auto it = SerializedModuleByName.find(ModuleName);
  return it != SerializedModuleByName.end()
             ? std::make_pair(it->first, it->second)
             : std::make_pair(std::string(), ModuleEntry{nullptr, 0});
}

void ModuleRegistry::registerModule(const std::string &ModuleName,
                                    const char *SerializedModule, size_t size) {
  SerializedModuleByName[ModuleName] = ModuleEntry{SerializedModule, size};
}

} // namespace soroka
