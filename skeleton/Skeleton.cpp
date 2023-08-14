#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

namespace {
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      errs() << "I saw a function called " << F.getName() << "!\n";
      return false;
    }
  };
}

// Pass info
char SkeletonPass::ID = 0;
static RegisterPass<SkeletonPass> X("SkeletonPass", "Skeleton pass", false,
                                    false);

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new SkeletonPass());
}

// Part of the old example, but gives:
// error: ‘RegisterStandardPasses’ does not name a type
// static RegisterStandardPasses
//    RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
//                   registerSkeletonPass);

#include "Skeleton.h"

OptimizeFooNewPM::Result
OptimizeFooNewPM::run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM) {
  llvm::errs() << "newPM opt-Foo\n";
  bool changed = false; // optimizeFncsWithFoo(M);
  return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
}

#include <llvm/Config/llvm-config.h>

#if LLVM_VERSION_MAJOR >= 16
#define private public
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"
#undef private
#else
#include "SCEV/ScalarEvolution.h"
#include "SCEV/ScalarEvolutionExpander.h"
#endif

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/MapVector.h"
#if LLVM_VERSION_MAJOR <= 16
#include "llvm/ADT/Optional.h"
#else
#include <optional>
#endif
#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"

#include "llvm/Passes/PassBuilder.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/Metadata.h"

#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Scalar.h"

#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/InlineAdvisor.h"
#include "llvm/Analysis/InlineCost.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/IR/AbstractCallSite.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"

#include "llvm/Transforms/Utils.h"

#if LLVM_VERSION_MAJOR >= 13
#include "llvm/Transforms/IPO/Attributor.h"
#include "llvm/Transforms/IPO/OpenMPOpt.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#endif

#include "llvm/Passes/PassPlugin.h"

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  llvm::errs() << "Running llvmGetPassPluginInfo()\n";
  return {
      LLVM_PLUGIN_API_VERSION, "FooNewPM", "v0.1", [](llvm::PassBuilder &PB) {
        auto Plugin = PassPlugin::Load(
            "/home/drehwald/prog/optPass/build/skeleton/libSkeletonPass.so");
        if (auto E = Plugin.takeError()) {
          // We must consume the error. Typically one of:
          // - return the error to our caller
          // - toString(), when logging
          // - consumeError(), to silently swallow the error
          // - handleErrors(), to distinguish error types
          errs() << "Problem with loading " << toString(std::move(E)) << "\n";
          return;
        }
        //// Register plugin extensions in PassBuilder.
        Plugin->registerPassBuilderCallbacks(PB);

        PB.registerPipelineParsingCallback(
            [](llvm::StringRef Name, llvm::ModulePassManager &MPM,
               llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
              llvm::errs() << "allways adding SkeletonPass\n";
              MPM.addPass(OptimizeFooNewPM(/*Begin*/ true));
              return true;
              if (Name == "SkeletonPass") {
                llvm::errs() << "Found SkeletonPass\n";
                MPM.addPass(OptimizeFooNewPM(/*Begin*/ true));
                return true;
              }
              llvm::errs() << "Not found\n";
              return false;
            });
      }};
}
