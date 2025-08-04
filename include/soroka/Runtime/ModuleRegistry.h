#pragma once
#include <unordered_map>
#include <utility>

namespace soroka {

struct ModuleEntry {
  const char *serializedData = nullptr;
  size_t size = 0;
};

class ModuleRegistry {
public:
  using container = std::unordered_map<const char *, ModuleEntry>;
  using iterator = typename container::iterator;
  using const_iterator = typename container::const_iterator;

  iterator begin() { return SerializedModuleByName.begin(); }
  iterator end() { return SerializedModuleByName.end(); }
  const_iterator begin() const { return SerializedModuleByName.begin(); }
  const_iterator end() const { return SerializedModuleByName.end(); }

  static ModuleRegistry &get();

  std::pair<const char *, ModuleEntry>
  getSerializedModule(const char *ModuleName);

  void registerModule(const char *ModuleName, const char *SerializedModule,
                      size_t size);

private:
  ModuleRegistry();

  std::unordered_map<const char *, ModuleEntry> SerializedModuleByName;
};
} // namespace soroka