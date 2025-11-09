#pragma once
#include <unordered_map>
#include <functional>
#include "ast/Type.h"

struct SymbolInfo
{
private:
    TypePtr m_Type;

public:
    SymbolInfo(TypePtr type);
    SymbolInfo(const SymbolInfo &) = delete;
    SymbolInfo(SymbolInfo &&) = default;

    const Type &getType() const { return *m_Type; }
};

using Scope = std::unordered_map<std::string, SymbolInfo>;

struct SymbolTable
{
private:
    std::vector<Scope> m_Scopes;

public:
    SymbolTable() = default;
    SymbolTable(const SymbolTable &) = delete;
    SymbolTable(SymbolTable &&) = delete;

    Scope &pushScope();
    void popScope();

    std::optional<Ref<const SymbolInfo>> getSymbol(const std::string &name) const;
    void addSymbol(std::string name, SymbolInfo symbol);
    bool hasSymbolInCurrentScope(const std::string name);
};