#pragma once
#include <iostream>
#include <unordered_map>
#include <memory>
#include "Declaration.h"

using NamespacePtr = std::shared_ptr<struct Namespace>;

struct Namespace 
{
private:
    std::string m_Name;
    //std::unordered_map<std::string, NamespacePtr> m_Children; No nested namespaces right now
    std::unordered_map<std::string, FunctionDeclaration> m_Functions;

public: 
    Namespace(std::string name);
    Namespace(const Namespace &) = delete;
    Namespace(Namespace &&) = delete;

    Namespace &operator=(const Namespace &) = delete; 
    Namespace &operator=(Namespace &&) = delete;

    void addFunction(FunctionDeclaration func);
    std::optional<Ref<const FunctionDeclaration>> getFunction(const std::string &name) const;

    friend std::ostream &operator<<(std::ostream &os, const Namespace &ns);
};