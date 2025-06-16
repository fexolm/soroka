#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/CodeGen/BackendUtil.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/CodeGen/ModuleBuilder.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Tooling/Tooling.h>
#include <llvm-20/llvm/IR/Constant.h>
#include <llvm-20/llvm/IR/DerivedTypes.h>
#include <llvm-20/llvm/IR/GlobalVariable.h>
#include <llvm-20/llvm/Support/MemoryBufferRef.h>
#include <llvm-20/llvm/Support/SourceMgr.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <memory>

namespace soroka {

class EmbedIrVisitor : public clang::RecursiveASTVisitor<EmbedIrVisitor> {
public:
  EmbedIrVisitor(clang::CompilerInstance &CI, llvm::StringRef ModuleName)
      : CI(CI) {}

  bool VisitFunctionDecl(clang::FunctionDecl *F) { return true; }

private:
  clang::CompilerInstance &CI;
  llvm::LLVMContext C;
};

class EmbedIrASTConsumer : public clang::ASTConsumer {
public:
  EmbedIrASTConsumer(clang::CompilerInstance &CI, llvm::StringRef ModuleName)
      : CI(CI), Visitor(CI, ModuleName) {}

  std::unique_ptr<llvm::Module> getMainModule() {
    clang::SourceManager &SM = CI.getSourceManager();
    clang::FileID FID = SM.getMainFileID();

    std::optional<llvm::MemoryBufferRef> MainFile = SM.getBufferOrNone(FID);

    if (!MainFile) {
      return nullptr;
    }

    llvm::SMDiagnostic SMD;

    llvm::LLVMContext C;

    std::unique_ptr<llvm::Module> M = llvm::parseIR(*MainFile, SMD, C);

    llvm::errs() << "file: " << MainFile->getBuffer() << "!!";

    SMD.print("soroka", llvm::errs());

    return M;
  }

  void HandleTranslationUnit(clang::ASTContext &Ctx) override {
    Visitor.TraverseDecl(Ctx.getTranslationUnitDecl());

    std::unique_ptr<llvm::Module> MainModule = getMainModule();

    if (!MainModule) {
      llvm::errs() << "Module is null\n";
      return;
    }

    MainModule->dump();
  }

private:
  clang::CompilerInstance &CI;
  EmbedIrVisitor Visitor;
};

class EmbedIrAction : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    llvm::StringRef InFile) override {
    std::string ModuleName = "jit:" + InFile.str();
    return std::unique_ptr<clang::ASTConsumer>(
        std::make_unique<EmbedIrASTConsumer>(CI, ModuleName));
  }

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &arg) override {
    return true;
  }

  ActionType getActionType() override { return AddAfterMainAction; }

  bool hasIRSupport() const override { return true; }
};
} // namespace soroka

static clang::FrontendPluginRegistry::Add<soroka::EmbedIrAction>
    X(/*Name=*/"embed-ir",
      /*Description=*/"Embed jitted functions IR to binary");