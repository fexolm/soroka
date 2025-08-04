#include "soroka/Runtime/FunctionRegistry.h"
#include <cstdio>

namespace soroka {
FunctionRegistry::FunctionRegistry() {}

FunctionRegistry &FunctionRegistry::get() {
  static FunctionRegistry FR;
  return FR;
}

const char *FunctionRegistry::getModule(const char *FunctionId) {
  auto it = ModuleByFuncId.find(FunctionId);
  return it != ModuleByFuncId.end() ? it->second : nullptr;
}

void FunctionRegistry::registerFunction(const char *FunctionId, const char *ModuleId) {
  ModuleByFuncId[FunctionId] = ModuleId;
}

} // namespace soroka
