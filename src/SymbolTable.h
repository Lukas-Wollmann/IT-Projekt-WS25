#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Typedef.h"

struct SymbolTable {
	struct Entry {
		std::string name;
		std::string typeName = "";
		size_t size = 0;
		size_t lineOfDeclaration = 0;
		std::vector<u32> lineOfUsage;
		size_t address = 0;

		bool operator==(const std::string &name) const;
		Entry(std::string name) : name(std::move(name)) {}
	};

	std::vector<Entry> entries;

  public:
	void newEntry(std::string name);
	std::optional<Entry &> getEntry(const std::string &name);
	std::optional<Entry &> getEntry(size_t index);
};