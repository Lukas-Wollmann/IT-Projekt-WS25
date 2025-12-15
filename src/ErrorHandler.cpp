#include "ErrorHandler.h"
#include <iostream>
#include <iomanip>

// ANSI Color Codes
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"

ErrorHandler::ErrorHandler(U8String filename, U8String sourceCode)
	: sourceCode(std::move(sourceCode))
	, filename(std::move(filename))
	, hasErrors(false)
{
}

void ErrorHandler::addError(ErrorLevel level, U8String message, SourceLoc location, size_t highlightLength)
{
	if (level == ErrorLevel::ERROR)
	{
		hasErrors = true;
	}

	errors.push_back({
		level,
		std::move(message),
		location,
		highlightLength
	});
}

void ErrorHandler::addTokenError(const Token &token, U8String message)
{
	addError(ErrorLevel::ERROR, std::move(message), token.loc, token.lexeme.length());
}

U8String ErrorHandler::getLineFromSource(size_t lineNumber) const
{
	size_t currentLine = 1;
	size_t lineStart = 0;
	
	for (size_t i = 0; i < sourceCode.data().size(); ++i)
	{
		if (sourceCode.data()[i] == u8'\n')
		{
			if (currentLine == lineNumber)
			{
				// Extract substring from lineStart to i
				return U8String(std::u8string(
					sourceCode.data().begin() + lineStart,
					sourceCode.data().begin() + i
				));
			}
			lineStart = i + 1;
			++currentLine;
		}
	}
	
	// Last line without \n
	if (currentLine == lineNumber && lineStart < sourceCode.data().size())
	{
		return U8String(std::u8string(
			sourceCode.data().begin() + lineStart,
			sourceCode.data().end()
		));
	}
	
	// Line not found
	return U8String("");
}

void ErrorHandler::printError(const ErrorMessage &error) const
{
	// Bestimme Farbe und Level-String
	std::string colorCode;
	std::string levelStr;
	
	switch (error.level)
	{
		case ErrorLevel::ERROR:
			colorCode = RED;
			levelStr = "error";
			break;
		case ErrorLevel::WARNING:
			colorCode = YELLOW;
			levelStr = "warning";
			break;
		case ErrorLevel::NOTE:
			colorCode = CYAN;
			levelStr = "note";
			break;
	}

	// Header: "error: message"
	std::cerr << colorCode << BOLD << levelStr << RESET
			  << BOLD << ": " << RESET << error.message << "\n";	// Dateiname und Position: "--> file.cpp:2:11"
	std::cerr << BOLD << BLUE << " --> " << RESET 
			  << filename << ":"
				  << error.location.line << ":" 
				  << (error.location.column + 1) << "\n";

		// Leere Zeile mit Pipe
		std::cerr << BOLD << BLUE << "  |" << RESET << "\n";

	// Code-Zeile
	U8String snippet = getLineFromSource(error.location.line);
	std::cerr << BOLD << BLUE << std::setw(3) << error.location.line << " | " << RESET
			  << snippet << "\n";		// Markierung (^^^^^)
		std::cerr << BOLD << BLUE << "  | " << RESET;
		for (size_t i = 0; i < error.location.column; ++i)
		{
			std::cerr << " ";
		}
		std::cerr << colorCode << BOLD;
		for (size_t i = 0; i < error.highlightLength; ++i)
		{
			std::cerr << "^";
		}
		std::cerr << RESET << "\n";
}

void ErrorHandler::printErrors() const
{
	for (const auto &error : errors)
	{
		printError(error);
		std::cout << "\n";
	}

	size_t errCount = errorCount();
	size_t warnCount = warningCount();

	if (errCount > 0 || warnCount > 0)
	{
		std::cout << BOLD;
		if (errCount > 0)
		{
			std::cout << RED << "error" << RESET << BOLD << ": could not compile due to "
			          << errCount << " error" << (errCount > 1 ? "s" : "");
		}
		if (warnCount > 0)
		{
			if (errCount > 0) std::cout << "; ";
			std::cout << YELLOW << warnCount << " warning" << (warnCount > 1 ? "s" : "") 
			          << " emitted" << RESET;
		}
		std::cout << RESET << "\n";
	}
}

size_t ErrorHandler::errorCount() const
{
	size_t count = 0;
	for (const auto &err : errors)
	{
		if (err.level == ErrorLevel::ERROR)
		{
			++count;
		}
	}
	return count;
}

size_t ErrorHandler::warningCount() const
{
	size_t count = 0;
	for (const auto &err : errors)
	{
		if (err.level == ErrorLevel::WARNING)
		{
			++count;
		}
	}
	return count;
}

void ErrorHandler::clear()
{
	errors.clear();
	hasErrors = false;
}
