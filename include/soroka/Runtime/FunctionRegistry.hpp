#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace soroka {
class FunctionRegistry {
public:
  static FunctionRegistry &get();

  std::string getModuleName(std::string_view FunctionId);
  void *getFunctionPtr(std::string_view FunctionId);

  void registerFunction(void *FunctionPtr, const char *FunctionId,
                        const char *ModuleId);

private:
  FunctionRegistry();

  std::unordered_map<std::string, std::string> ModuleByFuncId;
  std::unordered_map<void *, std::string> FuncIdByFuncPtr;
};
} // namespace soroka
