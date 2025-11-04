#include "SymbolTable.h"

#include <algorithm>

void SymbolTable::newEntry(std::string name) {
	entries.push_back(Entry(std::move(name)));
}

std::optional<SymbolTable::Entry &> SymbolTable::getEntry(const std::string &name) {
	auto it = std::find_if(entries.begin(), entries.end(), name);
	if (it != entries.end())
		return *it;
	return std::nullopt;
}

std::optional<SymbolTable::Entry &> SymbolTable::getEntry(size_t index) {
	if (index < entries.size())
		return entries.at(index);
	return std::nullopt;
}

bool SymbolTable::Entry::operator==(const std::string &name) const {
	return this->name == name;
}
