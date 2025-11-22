#pragma once
#include "ast/Type.h"

struct FunctionDeclaration
{
private:
    std::string m_Name;
    FunctionTypePtr m_Type;

public:
    FunctionDeclaration(std::string name, FunctionTypePtr type);
    FunctionDeclaration(const FunctionDeclaration &) = delete;
    FunctionDeclaration(FunctionDeclaration &&) = default;

    FunctionDeclaration &operator=(const FunctionDeclaration &) = delete;
    FunctionDeclaration &operator=(FunctionDeclaration &&) = delete;

    FunctionType &getType() const;
    const std::string &getName() const;
};