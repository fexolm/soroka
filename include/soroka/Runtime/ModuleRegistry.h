#pragma once
#include <unordered_map>

namespace soroka {
class ModuleRegistry {
public:
  static ModuleRegistry &get();

  const char *getFunctionName(void *Ptr);

  void registerFunction(void *Ptr, const char *Name);

private:
  ModuleRegistry();

  std::unordered_map<void *, const char *> NameByFuncPtr;
};
} // namespace soroka