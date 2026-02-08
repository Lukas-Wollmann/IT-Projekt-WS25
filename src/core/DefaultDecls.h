#pragma once
#include "Typedef.h"
#include "type/Type.h"

inline Map<U8String, type::FunctionTypePtr> s_DefaultDecls = {
		{u8"print_i32",
		 std::make_shared<type::FunctionType>(Vec<type::TypePtr>{std::make_shared<type::Typename>(
													  u8"i32")},
											  std::make_shared<type::UnitType>())}};