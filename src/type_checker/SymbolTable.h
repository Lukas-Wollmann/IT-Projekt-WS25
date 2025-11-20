#pragma once
#include <unordered_map>
#include <iostream>
#include <functional>
#include "ast/Type.h"
#include "Token.h"

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

using ScopePtr = std::shared_ptr<struct Scope>;
using WeakScopePtr = std::weak_ptr<struct Scope>;

struct Scope : std::enable_shared_from_this<Scope>
{
    friend struct TraversalContext;

private:
    std::unordered_map<std::string, SymbolInfo> m_Symbols;
    std::vector<ScopePtr> m_Children;
    WeakScopePtr m_Parent;

public:
    Scope(WeakScopePtr parent = {});
    Scope(const Scope &) = delete;
    Scope(Scope &&) = delete;

    Scope &operator=(const Scope &) = delete;
    Scope &operator=(Scope &&) = delete;

    ScopePtr createScope();
    ScopePtr getParent() const;
    void addSymbol(std::string name, SymbolInfo symbol);
    std::optional<Ref<const SymbolInfo>> getSymbol(const std::string &name) const;
    bool isSymbolDefinedInThisScope(const std::string name);

    void toString(std::ostream &os, size_t indent = 0) const;
};

struct TraversalContext 
{
private:
    ScopePtr m_Current;
    std::vector<size_t> m_ChildIndexStack;

public:
    explicit TraversalContext(ScopePtr root);

    ScopePtr enterScope();
    ScopePtr exitScope();
    ScopePtr getCurrent() const { return m_Current; }
};

struct SymbolTable
{
private:
    ScopePtr m_GlobalScope, m_Current;

public:
    SymbolTable();
    SymbolTable(const SymbolTable &) = delete;
    SymbolTable(SymbolTable &&) = delete;

    SymbolTable &operator=(const SymbolTable &) = delete; 
    SymbolTable &operator=(SymbolTable &&) = delete;

    ScopePtr createScope();
    void exitScope();
    void resetScopeIterationState();

    void addSymbol(std::string name, SymbolInfo symbol);
    std::optional<Ref<const SymbolInfo>> getSymbol(const std::string &name) const;
    bool isSymbolDefinedInCurrentScope(const std::string name);

    ScopePtr getGlobalScope() const;
};

struct TypeError
{
public:
    std::string m_Msg;
    SourceLoc m_Loc;

    TypeError(std::string msg);

    friend std::ostream &operator<<(std::ostream &os, const TypeError &err);
};

struct TypeCheckingContext 
{
private:
    SymbolTable m_SymbolTable;
    std::vector<TypeError> m_Errors;

public:
    SymbolTable &getSymbolTable() { return m_SymbolTable; }
    void addError(TypeError error) { m_Errors.push_back(std::move(error)); }
};