#include "Operators.h"

U8String str(BinaryOpKind kind) {
	using enum BinaryOpKind;

	switch (kind) {
		case Addition:			 return u8"+";
		case Subtraction:		 return u8"-";
		case Multiplication:	 return u8"*";
		case Division:			 return u8"/";
		case Modulo:			 return u8"%";
		case Equality:			 return u8"==";
		case Inequality:		 return u8"!=";
		case LessThan:			 return u8"<";
		case GreaterThan:		 return u8">";
		case LessThanOrEqual:	 return u8"<=";
		case GreaterThanOrEqual: return u8">=";
		case LogicalAnd:		 return u8"&&";
		case LogicalOr:			 return u8"||";
		case BitwiseAnd:		 return u8"&";
		case BitwiseOr:			 return u8"|";
		case BitwiseXor:		 return u8"^";
		case LeftShift:			 return u8"<<";
		case RightShift:		 return u8">>";
		default:				 UNREACHABLE();
	}
}

U8String str(UnaryOpKind kind) {
	using enum UnaryOpKind;

	switch (kind) {
		case LogicalNot:  return u8"!";
		case BitwiseNot:  return u8"~";
		case Positive:	  return u8"+";
		case Negative:	  return u8"-";
		case Dereference: return u8"*";
		default:		  UNREACHABLE();
	}
}
