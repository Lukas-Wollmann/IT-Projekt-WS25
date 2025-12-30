#pragma once
#include "ast/AST.h"

namespace codegen {
    void generate(std::ostream &os, const ast::Module &module);
}