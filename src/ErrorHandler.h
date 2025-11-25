#pragma once

#include <string>
#include <vector>
#include "core/U8String.h"
#include "lexer/Token.h"

enum class ErrorLevel
{
	ERROR,
	WARNING,
	NOTE
};

struct ErrorMessage
{
	ErrorLevel level;
	U8String message;
	SourceLoc location;
	size_t highlightLength;
};

class ErrorHandler
{
  private:
	std::vector<ErrorMessage> errors;
	U8String sourceCode;
	U8String filename;
	bool hasErrors;

	// Hilfsfunktionen
	U8String getLineFromSource(size_t lineNumber) const;
	void printError(const ErrorMessage &error) const;

  public:
	ErrorHandler(U8String filename, U8String sourceCode);

	// Fehler hinzufügen
	void addError(ErrorLevel level, U8String message, SourceLoc location, 
	              size_t highlightLength = 1);

	// Fehler für Token hinzufügen
	void addTokenError(const Token &token, U8String message);

	// Alle Fehler im Rust-Stil ausgeben
	void printErrors() const;

	// Prüfen, ob Fehler vorhanden sind
	bool hasError() const { return hasErrors; }

	// Fehler zählen
	size_t errorCount() const;
	size_t warningCount() const;

	// Alle Fehler löschen
	void clear();
};
