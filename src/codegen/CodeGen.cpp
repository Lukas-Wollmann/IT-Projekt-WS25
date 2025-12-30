#include "CodeGen.h"
#include "CodeGenContext.h"
#include "StmtCodeGen.h"

#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/raw_ostream.h>

namespace codegen {
    void generate(std::ostream &os, const ast::Module &module) {
        CodeGenContext context(module.name);
        RValueCodeGen rValueCodeGen(context);
        LValueCodeGen lValueCodeGen(context, rValueCodeGen);
        StmtCodeGen stmtCodeGen(context, rValueCodeGen, lValueCodeGen);

        stmtCodeGen.dispatch(module);

        llvm::raw_os_ostream llvmOS(os);
		context.getLLVMModule().print(llvmOS, nullptr);
    }
}