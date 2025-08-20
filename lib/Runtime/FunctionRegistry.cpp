#include "soroka/Runtime/FunctionRegistry.hpp"

#include <cstdio>
#include <stdexcept>
#include <string>
#include <string_view>

namespace soroka {
FunctionRegistry::FunctionRegistry() {}

FunctionRegistry &FunctionRegistry::get() {
  static FunctionRegistry FR;
  return FR;
}

std::string FunctionRegistry::getModuleName(std::string_view FunctionId) {
  auto it = ModuleByFuncId.find(FunctionId.data());
  if (it == ModuleByFuncId.end()) {
    throw std::runtime_error("Function not found: " + std::string(FunctionId) +
                             "\n");
  }
  return it->second;
}

void *FunctionRegistry::getFunctionPtr(std::string_view FunctionId) {
  for (const auto &pair : FuncIdByFuncPtr) {
    if (pair.second == FunctionId) {
      return pair.first;
    }
  }
  return nullptr;
}

FunctionEntry FunctionRegistry::getFunctionModuleAndIdName(void *FunctionPtr) {
  auto it = FuncIdByFuncPtr.find(FunctionPtr);
  if (it == FuncIdByFuncPtr.end()) {
    throw std::runtime_error("Function pointer not found\n");
  }
  std::string moduleName = getModuleName(it->second);
  return {moduleName, it->second};
}

void FunctionRegistry::registerFunction(void *FunctionPtr,
                                        const char *FunctionId,
                                        const char *ModuleId) {
  ModuleByFuncId[FunctionId] = ModuleId;
  FuncIdByFuncPtr[FunctionPtr] = FunctionId;
}

} // namespace soroka
