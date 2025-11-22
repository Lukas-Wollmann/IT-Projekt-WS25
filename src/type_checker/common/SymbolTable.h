#pragma once
#include <unordered_map>
#include <iostream>
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

std::ostream &operator<<(std::ostream &os, const Scope &scope);

struct SymbolTable
{
private:
    std::vector<Scope> m_Scopes;

public:
    SymbolTable() = default;
    SymbolTable(const SymbolTable &) = delete;
    SymbolTable(SymbolTable &&) = delete;

    SymbolTable &operator=(const SymbolTable &) = delete; 
    SymbolTable &operator=(SymbolTable &&) = delete;

    Scope &enterScope();
    void exitScope();
    
    void addSymbol(std::string name, SymbolInfo symbol);
    std::optional<Ref<const SymbolInfo>> getSymbol(const std::string &name) const;
    bool isSymbolDefinedInCurrentScope(const std::string &name) const;

    friend std::ostream &operator<<(std::ostream &os, const SymbolTable &table);
};