#include "Doctest.h"
#include "mir/Lowerer.h"
#include "semantic/common/TypeCheckerContext.h"
#include "semantic/passes/TypeCheckingPass.h"

TEST_CASE("ExprLowerer: Sandbox") {
	LoweringContext ctx(u8"test");

	llvm::Type *size_t_Ty = ctx.getIRBuilder().getIntPtrTy(ctx.getLLVMModule().getDataLayout());
	llvm::Type *ptrTy = ctx.getIRBuilder().getPtrTy();
	llvm::Type *i32Ty = ctx.getIRBuilder().getInt32Ty();
	llvm::Type *voidTy = ctx.getIRBuilder().getVoidTy();

	llvm::FunctionType *mainTy = llvm::FunctionType::get(i32Ty, {}, false);
	llvm::FunctionType *spCreateTy = llvm::FunctionType::get(ptrTy, {size_t_Ty, ptrTy}, false);
	llvm::FunctionType *spCopyTy = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
	llvm::FunctionType *spDropTy = llvm::FunctionType::get(voidTy, {ptrTy}, false);

	ctx.getLLVMModule().getOrInsertFunction("__sp_create", spCreateTy);
	ctx.getLLVMModule().getOrInsertFunction("__sp_copy", spCopyTy);
	ctx.getLLVMModule().getOrInsertFunction("__sp_drop", spDropTy);

	auto *main = llvm::Function::Create(mainTy, llvm::GlobalValue::ExternalLinkage, "main",
										ctx.getLLVMModule());
	auto *entry = llvm::BasicBlock::Create(ctx.getLLVMContext(), "entry", main);
	ctx.getIRBuilder().SetInsertPoint(entry);
	ExprLowerer exprLowerer(ctx);
	U8String code(u8"");

	ErrorHandler errhndl(u8"", code);
	sem::TypeCheckerContext tcctx(errhndl);
	sem::TypeCheckingPass pass(tcctx);

	ast::Assignment expr(AssignmentKind::Simple,
						 std::make_unique<ast::UnaryExpr>(
								 UnaryOpKind::Dereference,
								 std::make_unique<ast::HeapAlloc>(
										 std::make_shared<type::PointerType>(
												 std::make_shared<type::Typename>(u8"i32")),
										 std::make_unique<ast::HeapAlloc>(
												 std::make_shared<type::Typename>(u8"i32"),
												 std::make_unique<ast::IntLit>(12)))),

						 std::make_unique<ast::HeapAlloc>(std::make_shared<type::Typename>(u8"i32"),
														  std::make_unique<ast::IntLit>(34)));

	ast::HeapAlloc expr2(std::make_shared<type::PointerType>(
								 std::make_shared<type::Typename>(u8"i32")),
						 std::make_unique<ast::HeapAlloc>(std::make_shared<type::Typename>(u8"i32"),
														  std::make_unique<ast::IntLit>(12)));

	pass.dispatch(expr2);
	exprLowerer.lowerExpr(expr2);
	exprLowerer.emitExprCleanup();
	ctx.getIRBuilder().CreateRet(llvm::ConstantInt::get(i32Ty, 0));
	ctx.getLLVMModule().print(llvm::errs(), nullptr);
}
