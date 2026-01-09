#pragma once

#include <sstream>
#include <string>

#include "lexer/Token.h"

class syntax_error : public std::exception {
private:
	std::string message;

public:
	syntax_error(const char *msg)
		: message(msg) {}

	syntax_error(std::string msg)
		: message(msg) {}

	const char *what() const noexcept {
		return message.c_str();
	}
};