#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#include <iostream>
#include <memory>

int main() {
    llvm::LLVMContext context;
    auto module = std::make_unique<llvm::Module>("my_module", context);

    llvm::IRBuilder<> builder(context);

    // int foo() { return 42; }
    auto funcType = llvm::FunctionType::get(builder.getInt32Ty(), false);
    auto fooFunc = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "foo", module.get());

    auto entry = llvm::BasicBlock::Create(context, "entry", fooFunc);
    builder.SetInsertPoint(entry);
    builder.CreateRet(builder.getInt32(42));

    // Optional: verify module
    if (llvm::verifyModule(*module, &llvm::errs())) {
        std::cerr << "Module verification failed!\n";
        return 1;
    }

    module->print(llvm::outs(), nullptr);
    

    std::cout << "fertig";
    return 0;
}
