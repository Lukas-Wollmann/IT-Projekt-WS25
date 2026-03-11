#pragma once
#include "Typedef.h"
#include "type/TypeFactory.h"

inline Map<U8String, FunctionType *> s_DefaultDecls = {
		{u8"print_i32", TypeFactory::getFunction({TypeFactory::getI32()}, TypeFactory::getUnit())},
		{u8"print_bool",
		 TypeFactory::getFunction({TypeFactory::getBool()}, TypeFactory::getUnit())},
		{u8"print_char",
		 TypeFactory::getFunction({TypeFactory::getChar()}, TypeFactory::getUnit())},
		{u8"print_newline", TypeFactory::getFunction({}, TypeFactory::getUnit())},
		{u8"read_i32", TypeFactory::getFunction({}, TypeFactory::getI32())},
		{u8"read_bool", TypeFactory::getFunction({}, TypeFactory::getBool())},
		{u8"read_char", TypeFactory::getFunction({}, TypeFactory::getChar())},
};