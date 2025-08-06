#include "soroka/Runtime/FunctionRegistry.hpp"

#include <cstdio>
#include <string>

namespace soroka {
FunctionRegistry::FunctionRegistry() {}

FunctionRegistry &FunctionRegistry::get() {
  static FunctionRegistry FR;
  return FR;
}

const std::string &
FunctionRegistry::getModuleName(const std::string &FunctionId) {
  auto it = ModuleByFuncId.find(FunctionId);
  static const std::string empty_string;
  return it != ModuleByFuncId.end() ? it->second : empty_string;
}

void *FunctionRegistry::getFunctionPtr(const std::string &FunctionId) {
  for (const auto &pair : FuncIdByFuncPtr) {
    if (pair.second == FunctionId) {
      return pair.first;
    }
  }
  return nullptr;
}

void FunctionRegistry::registerFunction(void *FunctionPtr,
                                        const std::string &FunctionId,
                                        const std::string &ModuleId) {
  ModuleByFuncId[FunctionId] = ModuleId;
  FuncIdByFuncPtr[FunctionPtr] = FunctionId;
}

} // namespace soroka
