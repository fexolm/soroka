#include "soroka/Runtime/FunctionRegistry.hpp"

#include <cstdio>
#include <string>

namespace soroka {
FunctionRegistry::FunctionRegistry() {}

FunctionRegistry &FunctionRegistry::get() {
  static FunctionRegistry FR;
  return FR;
}

const std::string
FunctionRegistry::getModuleName(const std::string &FunctionId) {
  auto it = ModuleByFuncId.find(FunctionId);
  return it != ModuleByFuncId.end() ? it->second : std::string();
}

void FunctionRegistry::registerFunction(const std::string &FunctionId,
                                        const std::string &ModuleId) {
  ModuleByFuncId[FunctionId] = ModuleId;
}

} // namespace soroka
