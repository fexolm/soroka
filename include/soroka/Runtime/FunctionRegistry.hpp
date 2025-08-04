#pragma once
#include <string>
#include <unordered_map>

namespace soroka {
class FunctionRegistry {
public:
  static FunctionRegistry &get();

  const std::string getModuleName(const std::string &FunctionId);

  void registerFunction(const std::string &FunctionId,
                        const std::string &ModuleId);

private:
  FunctionRegistry();

  std::unordered_map<std::string, std::string> ModuleByFuncId;
};
} // namespace soroka
