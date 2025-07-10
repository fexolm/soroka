#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <sys/types.h>

void printIRFromBitcode(const char *data, size_t size) {
  auto buffer = llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(data, size),
                                                 "kek_bytes", false);
  llvm::LLVMContext context;
  auto module = llvm::parseBitcodeFile(buffer->getMemBufferRef(), context);
  if (!module) {
    llvm::errs() << "Wrong read bytecode!\n";
    return;
  }
  (*module)->print(llvm::outs(), nullptr);
}

int main() { std::cout << "PRINT IR FROM BITCODE" << std::endl; }