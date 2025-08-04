#pragma once
#include <unordered_map>

namespace soroka {
class FunctionRegistry {
public:
  static FunctionRegistry &get();

  const char *getModule(const char *FunctionId);

  void registerFunction(const char *FunctionId, const char *ModuleId);

private:
  FunctionRegistry();

  std::unordered_map<const char *, const char *> ModuleByFuncId;
};
} // namespace soroka
