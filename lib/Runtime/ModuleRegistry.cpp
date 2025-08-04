#include "soroka/Runtime/ModuleRegistry.h"

#include <cstddef>
#include <cstdio>
#include <utility>

namespace soroka {
ModuleRegistry::ModuleRegistry() {}

ModuleRegistry &ModuleRegistry::get() {
  static ModuleRegistry MR;
  return MR;
}

std::pair<const char *, ModuleEntry>
ModuleRegistry::getSerializedModule(const char *ModuleName) {
  auto it = SerializedModuleByName.find(ModuleName);
  return it != SerializedModuleByName.end()
             ? std::make_pair(it->first, it->second)
             : std::make_pair(nullptr, ModuleEntry{nullptr, 0});
}

void ModuleRegistry::registerModule(const char *ModuleName,
                                    const char *SerializedModule, size_t size) {
  SerializedModuleByName[ModuleName] = ModuleEntry{SerializedModule, size};
}

} // namespace soroka
