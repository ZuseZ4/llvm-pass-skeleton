#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassPlugin.h"

#include "llvm/Passes/PassBuilder.h"
namespace llvm {
class ModulePass;
}

bool optimizeFncsWithFoo(llvm::Module &M);

llvm::ModulePass *createOptimizeFooPass(bool Begin);

class OptimizeFooNewPM final : public llvm::PassInfoMixin<OptimizeFooNewPM> {
  friend struct llvm::PassInfoMixin<OptimizeFooNewPM>;

private:
  bool Begin;

public:
  using Result = llvm::PreservedAnalyses;
  OptimizeFooNewPM(bool Begin) : Begin(Begin) {}

  Result run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM);

  static bool isRequired() { return true; }
};
