#include <fstream>
#include <iostream>
#include <memory>

#include "ast/PrintVisitor.h"
#include "codegen/CodeGen.h"
#include "semantic/passes/ExplorationPass.h"
#include "semantic/passes/TypeCheckingPass.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"

using namespace ast;
using namespace type;
using namespace semantic;
using namespace codegen;


#define MAKE(t, ...) std::make_unique<t>(__VA_ARGS__)
#define MOVE(t)		 std::move(t)

int main(const int argc, const char *argv[]) {
    std::string buf; 
    std::getline(std::cin, buf);

    U8String source(buf);

    Lexer lexer(std::move(source));
    auto tokens = lexer.tokenize();

    for (auto tok : tokens)
        std::cout << tok << std::endl;

    Parser parser(tokens, "test-module");
    auto ast = parser.parse();

    for (auto err : parser.errors)
        std::cout << err << std::endl;

    std::cout << *ast << std::endl;
}


#if 0
int main() {
	Vec<Box<Stmt>> stmts;

	// n = 5, res = 1, i = 1
	stmts.push_back(MAKE(VarDef, u8"n", MAKE(Typename, u8"i32"), MAKE(IntLit, 5)));
	stmts.push_back(MAKE(VarDef, u8"res", MAKE(Typename, u8"i32"), MAKE(IntLit, 1)));
	stmts.push_back(MAKE(VarDef, u8"i", MAKE(Typename, u8"i32"), MAKE(IntLit, 1)));

	// outer loop: while(i <= n)
	auto cond_outer = MAKE(BinaryExpr, BinaryOpKind::LessThanOrEqual, MAKE(VarRef, u8"i"),
						   MAKE(VarRef, u8"n"));
	Vec<Box<Stmt>> body_outer;

	// inner loop variables
	body_outer.push_back(MAKE(VarDef, u8"j", MAKE(Typename, u8"i32"), MAKE(IntLit, 1)));
	body_outer.push_back(MAKE(VarDef, u8"tmp", MAKE(Typename, u8"i32"), MAKE(IntLit, 0)));

	// inner loop: while(j <= i)
	auto cond_inner = MAKE(BinaryExpr, BinaryOpKind::LessThanOrEqual, MAKE(VarRef, u8"j"),
						   MAKE(VarRef, u8"i"));
	Vec<Box<Stmt>> body_inner;
	body_inner.push_back(MAKE(Assignment, AssignmentKind::Simple, MAKE(VarRef, u8"tmp"),
							  MAKE(BinaryExpr, BinaryOpKind::Addition, MAKE(VarRef, u8"tmp"),
								   MAKE(VarRef, u8"j"))));
	body_inner.push_back(
			MAKE(Assignment, AssignmentKind::Simple, MAKE(VarRef, u8"j"),
				 MAKE(BinaryExpr, BinaryOpKind::Addition, MAKE(VarRef, u8"j"), MAKE(IntLit, 1))));
	auto inner_while = MAKE(WhileStmt, MOVE(cond_inner), MAKE(BlockStmt, MOVE(body_inner)));
	body_outer.push_back(MOVE(inner_while));

	// res = res * tmp
	body_outer.push_back(MAKE(Assignment, AssignmentKind::Simple, MAKE(VarRef, u8"res"),
							  MAKE(BinaryExpr, BinaryOpKind::Multiplication, MAKE(VarRef, u8"res"),
								   MAKE(VarRef, u8"tmp"))));

	// i = i + 1
	body_outer.push_back(
			MAKE(Assignment, AssignmentKind::Simple, MAKE(VarRef, u8"i"),
				 MAKE(BinaryExpr, BinaryOpKind::Addition, MAKE(VarRef, u8"i"), MAKE(IntLit, 1))));

	auto outer_while = MAKE(WhileStmt, MOVE(cond_outer), MAKE(BlockStmt, MOVE(body_outer)));

	stmts.push_back(MOVE(outer_while));
	stmts.push_back(MAKE(ReturnStmt, MAKE(VarRef, u8"res")));

	// function main
	Vec<Param> params;
	auto mainFunc = MAKE(FuncDecl, u8"main", MOVE(params), MAKE(Typename, u8"i32"),
						 MAKE(BlockStmt, MOVE(stmts)));

	// module
	Vec<Box<FuncDecl>> funcs;
	funcs.push_back(MOVE(mainFunc));
	auto module = MAKE(Module, u8"complex_test", MOVE(funcs));

	// type check
	TypeCheckerContext ctx;
	ExplorationPass ep(ctx);
	TypeCheckingPass tc(ctx);
	ep.dispatch(*module);
	tc.dispatch(*module);

	std::cout << *module << std::endl;

	// generate code
	if (!ctx.getErrors().empty()) {
		for (auto &err : ctx.getErrors())
			std::cout << err << std::endl;
		return 1;
	}
	std::ofstream file("out.ll");
	CodeGen::generate(file, *module);
#endif
#if 0
    // ======== iterative Fibonacci ========

    // Variable declarations
    Vec<Box<Stmt>> stmts;
    stmts.push_back(MAKE(VarDef, u8"n", MAKE(Typename, u8"i32"), MAKE(IntLit, 20)));
    stmts.push_back(MAKE(VarDef, u8"a", MAKE(Typename, u8"i32"), MAKE(IntLit, 0)));
    stmts.push_back(MAKE(VarDef, u8"b", MAKE(Typename, u8"i32"), MAKE(IntLit, 1)));
    stmts.push_back(MAKE(VarDef, u8"temp", MAKE(Typename, u8"i32"), MAKE(IntLit, 0)));
    stmts.push_back(MAKE(VarDef, u8"i", MAKE(Typename, u8"i32"), MAKE(IntLit, 2)));

    // while(i <= n) { ... }
    auto cond = MAKE(BinaryExpr, BinaryOpKind::LessThanOrEqual, MAKE(VarRef, u8"i"), MAKE(VarRef, u8"n"));

    Vec<Box<Stmt>> body;
    body.push_back(MAKE(Assignment, AssignmentKind::Simple, MAKE(VarRef, u8"temp"), MAKE(VarRef, u8"b")));
    body.push_back(MAKE(Assignment, AssignmentKind::Simple, MAKE(VarRef, u8"b"),
                         MAKE(BinaryExpr, BinaryOpKind::Addition, MAKE(VarRef, u8"a"), MAKE(VarRef, u8"b"))));
    body.push_back(MAKE(Assignment, AssignmentKind::Simple, MAKE(VarRef, u8"a"), MAKE(VarRef, u8"temp")));
    body.push_back(MAKE(Assignment, AssignmentKind::Simple, MAKE(VarRef, u8"i"),
                         MAKE(BinaryExpr, BinaryOpKind::Addition, MAKE(VarRef, u8"i"), MAKE(IntLit, 1))));

    auto whileStmt = MAKE(WhileStmt, MOVE(cond), MAKE(BlockStmt, MOVE(body)));
    stmts.push_back(MOVE(whileStmt));

    // return b
    stmts.push_back(MAKE(ReturnStmt, MAKE(VarRef, u8"b")));

    // Function main()
    Vec<Param> params; // no parameters
    auto mainFunc = MAKE(FuncDecl, u8"main", MOVE(params), MAKE(Typename, u8"i32"), MAKE(BlockStmt, MOVE(stmts)));

    // Module
    Vec<Box<FuncDecl>> funcs;
    funcs.push_back(MOVE(mainFunc));
    auto module = MAKE(Module, u8"fib_module", MOVE(funcs));

    // Type checking
    TypeCheckerContext ctx;
    ExplorationPass ep(ctx);
    TypeCheckingPass tc(ctx);

    ep.dispatch(*module);
    tc.dispatch(*module);

    if (!ctx.getErrors().empty()) {
        for (auto &err : ctx.getErrors())
            std::cout << err << std::endl;
        return 1;
    }

    // Code generation
    std::ofstream file("out.ll");
    CodeGen::generate(file, *module);
#endif

#if 0
    //  =========== pow.code ===========
    // 
    //  func pow(base: i32, exponent: i32) -> i32 {
    //      res: i32 = 1    
    //      while(exponent > 0) {
    //          res = res * base;
    //          exponent = exponent - 1;
    //      }
    //      return res
    //  }
    
    auto res = MAKE(VarDef, 
        u8"res", 
        MAKE(Typename, u8"i32"), 
        MAKE(IntLit, 1)
    );
    auto cond = MAKE(BinaryExpr, 
        BinaryOpKind::GreaterThan, 
        MAKE(VarRef, u8"exponent"), 
        MAKE(IntLit, 0)
    );
    
    Vec<Box<Stmt>> body;
    body.push_back(
        MAKE(Assignment,
            AssignmentKind::Simple,
            MAKE(VarRef, u8"res"), 
            MAKE(BinaryExpr, 
                BinaryOpKind::Multiplication, 
                MAKE(VarRef, u8"res"), 
                MAKE(VarRef, u8"base")
            )
        )
    );
    body.push_back(
        MAKE(Assignment,
            AssignmentKind::Simple,
            MAKE(VarRef, u8"exponent"), 
            MAKE(BinaryExpr, 
                BinaryOpKind::Subtraction, 
                MAKE(VarRef, u8"exponent"), 
                MAKE(IntLit, 1)
            )
        )
    );

    auto while_ = MAKE(WhileStmt, MOVE(cond), MAKE(BlockStmt, MOVE(body)));

    Vec<Box<Stmt>> stmts;
    stmts.push_back(
        MAKE(Assignment,
            AssignmentKind::Simple,
            MAKE(VarRef, u8"base"),
            MAKE(IntLit, 3) 
        )
    );
    stmts.push_back(
        MAKE(Assignment,
            AssignmentKind::Simple,
            MAKE(VarRef, u8"exponent"),
            MAKE(IntLit, 5) 
        )
    );
    stmts.push_back(MOVE(res));
    stmts.push_back(MOVE(while_));
    stmts.push_back(MAKE(ReturnStmt, MAKE(VarRef, u8"res")));

    Vec<Box<FuncDecl>> funcs;
    Vec<Param> params;
    params.push_back({u8"base", MAKE(Typename, u8"i32")});
    params.push_back({u8"exponent", MAKE(Typename, u8"i32")});

    funcs.push_back(
        MAKE(FuncDecl, 
            u8"main", 
            MOVE(params), 
            MAKE(Typename, u8"i32"), 
            MAKE(BlockStmt, MOVE(stmts))
        )
    );

    auto module = MAKE(Module, u8"math_util", MOVE(funcs));

    TypeCheckerContext ctx;
    ExplorationPass ep(ctx);
    TypeCheckingPass tc(ctx);

    ep.dispatch(*module);
    tc.dispatch(*module);

    if (!ctx.getErrors().empty()) {
        for (auto err : ctx.getErrors())
            std::cout << err << std::endl;

        return 1;
    }

    std::ofstream file("out.ll");
    CodeGen::generate(file, *module);
#endif
#if 0
    /*

    TypePtr i32Type = std::make_shared<Typename>(u8"i32");

    Vec<Box<Stmt>> stmts;

    // int n = 20;
    stmts.push_back(std::make_unique<VarDef>(
        u8"n",
        std::make_unique<Typename>(u8"i32"),
        std::make_unique<IntLit>(20)
    ));

    // int a = 0;
    stmts.push_back(std::make_unique<VarDef>(
        u8"a",
        i32Type,
        std::make_unique<IntLit>(0)
    ));

    // int b = 1;
    stmts.push_back(std::make_unique<VarDef>(
        u8"b",
        i32Type,
        std::make_unique<IntLit>(1)
    ));

    // int temp = 0;  <-- Declare temp outside loop
    stmts.push_back(std::make_unique<VarDef>(
        u8"temp",
        i32Type,
        std::make_unique<IntLit>(0)
    ));

    // int i = 2;
    stmts.push_back(std::make_unique<VarDef>(
        u8"i",
        i32Type,
        std::make_unique<IntLit>(2)
    ));

    // while (i <= n) { ... }
    auto condWhile = std::make_unique<BinaryExpr>(
        BinaryOpKind::LessThanOrEqual,
        std::make_unique<VarRef>(u8"i"),
        std::make_unique<VarRef>(u8"n")
    );

    Vec<Box<Stmt>> bodyWhile;

    // temp = b;
    bodyWhile.push_back(std::make_unique<Assignment>(
        AssignmentKind::Simple,
        std::make_unique<VarRef>(u8"temp"),
        std::make_unique<VarRef>(u8"b")
    ));

    // b = a + b;
    bodyWhile.push_back(std::make_unique<Assignment>(
        AssignmentKind::Simple,
        std::make_unique<VarRef>(u8"b"),
        std::make_unique<BinaryExpr>(
            BinaryOpKind::Addition,
            std::make_unique<VarRef>(u8"a"),
            std::make_unique<VarRef>(u8"b")
        )
    ));

    // a = temp;
    bodyWhile.push_back(std::make_unique<Assignment>(
        AssignmentKind::Simple,
        std::make_unique<VarRef>(u8"a"),
        std::make_unique<VarRef>(u8"temp")
    ));

    // i = i + 1;
    bodyWhile.push_back(std::make_unique<Assignment>(
        AssignmentKind::Simple,
        std::make_unique<VarRef>(u8"i"),
        std::make_unique<BinaryExpr>(
            BinaryOpKind::Addition,
            std::make_unique<VarRef>(u8"i"),
            std::make_unique<IntLit>(1)
        )
    ));

    auto whileBlock = std::make_unique<BlockStmt>(std::move(bodyWhile));
    stmts.push_back(std::make_unique<WhileStmt>(
        std::move(condWhile),
        std::move(whileBlock)
    ));

    // return b;
    stmts.push_back(std::make_unique<ReturnStmt>(
        std::make_unique<VarRef>(u8"b")
    ));

    // function body
    auto body = std::make_unique<BlockStmt>(std::move(stmts));

    // function declaration
    Vec<Param> params; // no parameters needed
    auto func = std::make_unique<FuncDecl>(
        U8String("main"),
        std::move(params),
        i32Type,
        std::move(body)
    );

    Vec<Box<FuncDecl>> funcs;
    funcs.push_back(std::move(func));
    auto module = std::make_unique<Module>(u8"fib_module", std::move(funcs));
    */

    auto node = std::make_unique<FuncCall>(std::make_unique<IntLit>(5), Vec<Box<Expr>>{});
    
    Vec<Box<Stmt>> stmts;
    stmts.push_back(std::move(node));

    auto func = std::make_unique<FuncDecl>(
        u8"test", 
        Vec<Param>{},
        std::make_shared<UnitType>(),
        std::make_unique<BlockStmt>(std::move(stmts))
    );

    Vec<Box<FuncDecl>> funcs;
    funcs.push_back(std::move(func));

    auto module = std::make_unique<Module>(u8"test-module", std::move(funcs));


    TypeCheckerContext ctx;
    ExplorationPass ep(ctx);
    TypeCheckingPass tc(ctx);

    ep.dispatch(*module);
    tc.dispatch(*module);

    if (!ctx.getErrors().empty()) {
        for (auto &err : ctx.getErrors())
            std::cout << err << std::endl;
        return 0;
    }

    // Code generation
    std::ofstream file("fib.ll");
    CodeGen cg(file);
    cg.dispatch(*node);

    return 0;
}
#endif
