#include "soroka/Runtime/ModuleRegistry.h"

namespace soroka {
ModuleRegistry::ModuleRegistry() {}

ModuleRegistry &ModuleRegistry::get() {
  static ModuleRegistry MR;
  return MR;
}

const char *ModuleRegistry::getFunctionName(void *Ptr) {
  return NameByFuncPtr[Ptr];
}

void ModuleRegistry::registerFunction(void *Ptr, const char *Name) {
  NameByFuncPtr[Ptr] = Name;
}

extern "C" void soroka_register_module(void *Ptr, const char *Name) {
  ModuleRegistry::get().registerFunction(Ptr, Name);
}

} // namespace soroka
