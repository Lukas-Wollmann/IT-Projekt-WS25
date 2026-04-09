#include "ErrorHandler.h"

#include <iomanip>
#include <iostream>

// ANSI Color Codes
#define RESET  "\033[0m"
#define BOLD   "\033[1m"
#define RED	   "\033[31m"
#define YELLOW "\033[33m"
#define BLUE   "\033[34m"
#define CYAN   "\033[36m"

ErrorHandler::ErrorHandler(U8String filename, const U8String &sourceCode)
	: sourceCode(sourceCode)
	, filename(std::move(filename))
	, hasErrors(false) {}

void ErrorHandler::addError(U8String message, SourceLoc loc, ErrorLevel level) {
	if (level == ErrorLevel::ERROR)
		hasErrors = true;

	errors.push_back({level, std::move(message), loc, loc.length});
}

U8String ErrorHandler::getLineFromSource(size_t lineNumber) const {
	size_t currentLine = 1;
	size_t lineStart = 0;

	for (size_t i = 0; i < sourceCode.data().size(); ++i) {
		if (sourceCode.data()[i] == u8'\n') {
			if (currentLine == lineNumber) {
				// Extract substring from lineStart to i
				return U8String(std::u8string(sourceCode.data().begin() + lineStart,
											  sourceCode.data().begin() + i));
			}
			lineStart = i + 1;
			++currentLine;
		}
	}

	// Last line without \n
	if (currentLine == lineNumber && lineStart < sourceCode.data().size()) {
		return U8String(
				std::u8string(sourceCode.data().begin() + lineStart, sourceCode.data().end()));
	}

	// Line not found
	return U8String("");
}

size_t ErrorHandler::getLineNumberWidth() const {
	if (errors.empty()) {
		return 1;
	}

	// Find maximum line number
	size_t maxLine = 0;
	for (const auto &error : errors) {
		if (error.location.line > maxLine) {
			maxLine = error.location.line;
		}
	}

	// Calculate number of digits
	size_t width = 0;
	size_t temp = maxLine;
	do {
		++width;
		temp /= 10;
	} while (temp > 0);

	return width;
}

void ErrorHandler::printError(const ErrorMessage &error) const {
	size_t lineWidth = getLineNumberWidth();

	std::string colorCode;
	std::string levelStr;

	switch (error.level) {
		case ErrorLevel::ERROR:
			colorCode = RED;
			levelStr = "ERROR";
			break;
		case ErrorLevel::WARNING:
			colorCode = YELLOW;
			levelStr = "WARNING";
			break;
		case ErrorLevel::NOTE:
			colorCode = CYAN;
			levelStr = "NOTE";
			break;
	}

	// Header: "error: message"
	std::cerr << colorCode << BOLD << levelStr << RESET << ": " << error.message << RESET << "\n";

	// Source location: "--> file.cpp:2:11" (1-based)
	std::cerr << BOLD << BLUE << " --> " << RESET << filename << ":" << error.location.line << ":"
			  << error.location.column << "\n";

	// Empty line with line number column for alignment
	std::cerr << BOLD << BLUE << std::setw(lineWidth) << "" << " |" << RESET << "\n";

	// Source line
	U8String snippet = getLineFromSource(error.location.line);
	std::cerr << BOLD << BLUE << std::setw(lineWidth) << error.location.line << " | " << RESET
			  << snippet << "\n";

	// marker line (^^^^^)
	std::cerr << BOLD << BLUE << std::setw(lineWidth) << "" << " | " << RESET;

	// Calculate the number of spaces before the first ^ and the number of ^
	size_t snippetLength = snippet.length();
	size_t zeroBasedColumn = (error.location.column > 0) ? error.location.column - 1 : 0;

	// Bounds check for column number
	if (zeroBasedColumn > snippetLength) {
		zeroBasedColumn = snippetLength;
	}

	for (size_t i = 0; i < zeroBasedColumn; ++i) {
		std::cerr << " ";
	}

	// Minimum highlight length von 1
	size_t safeHighlightLength = (error.highlightLength == 0) ? 1 : error.highlightLength;

	// Highlight length should not exceed the remaining characters in the line
	size_t remainingLength = snippetLength - zeroBasedColumn;
	if (safeHighlightLength > remainingLength && remainingLength > 0) {
		safeHighlightLength = remainingLength;
	}

	// Print the ^ markers
	std::cerr << colorCode << BOLD;
	for (size_t i = 0; i < safeHighlightLength; ++i) {
		std::cerr << "^";
	}
	std::cerr << RESET << "\n";
}

void ErrorHandler::printErrors() const {
	for (const auto &error : errors) {
		printError(error);
		std::cerr << "\n";
	}

	size_t errCount = errorCount();
	size_t warnCount = warningCount();

	if (errCount > 0 || warnCount > 0) {
		std::cerr << RESET;
		if (errCount > 0) {
			std::cerr << RED << BOLD << errCount << " error" << (errCount > 1 ? "s" : "")
					  << " emitted.\n"
					  << RESET;
		}
		if (warnCount > 0) {
			std::cerr << YELLOW << BOLD << warnCount << " warning" << (warnCount > 1 ? "s" : "")
					  << " emitted.\n"
					  << RESET;
		}
		std::cerr << RESET << std::flush;
	}
}

size_t ErrorHandler::errorCount() const {
	size_t count = 0;
	for (const auto &err : errors) {
		if (err.level == ErrorLevel::ERROR) {
			++count;
		}
	}
	return count;
}

size_t ErrorHandler::warningCount() const {
	size_t count = 0;
	for (const auto &err : errors) {
		if (err.level == ErrorLevel::WARNING) {
			++count;
		}
	}
	return count;
}

void ErrorHandler::clear() {
	errors.clear();
	hasErrors = false;
}
