#include "Operators.h"

std::ostream &operator<<(std::ostream &os, const BinaryOpKind kind) {
	using enum BinaryOpKind;

	switch (kind) {
		case Addition:			 return os << "+";
		case Subtraction:		 return os << "-";
		case Multiplication:	 return os << "*";
		case Division:			 return os << "/";
		case Modulo:			 return os << "%";
		case Equality:			 return os << "==";
		case Inequality:		 return os << "!=";
		case LessThan:			 return os << "<";
		case GreaterThan:		 return os << ">";
		case LessThanOrEqual:	 return os << "<=";
		case GreaterThanOrEqual: return os << ">=";
		case LogicalAnd:		 return os << "&&";
		case LogicalOr:			 return os << "||";
		case BitwiseAnd:		 return os << "&";
		case BitwiseOr:			 return os << "|";
		case BitwiseXor:		 return os << "^";
		case LeftShift:			 return os << "<<";
		case RightShift:		 return os << ">>";
		default:				 UNREACHABLE();
	}
}

std::ostream &operator<<(std::ostream &os, const UnaryOpKind kind) {
	using enum UnaryOpKind;

	switch (kind) {
		case LogicalNot:  return os << "!";
		case BitwiseNot:  return os << "~";
		case Positive:	  return os << "+";
		case Negative:	  return os << "-";
		case Dereference: return os << "*";
		default:		  UNREACHABLE();
	}
}