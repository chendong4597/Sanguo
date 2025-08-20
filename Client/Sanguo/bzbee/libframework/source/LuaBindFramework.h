/*!
 * \file LuaBindFramework.h
 *
 * \author 
 * \date 
 *
 * 
 */
#pragma once
#include <lua/LuaPlus/LuaPlus.h>

class LuaBindFramework
{
public:
    static bool bindCFunction(LuaPlus::LuaObject& luaObj);
	static bool bindMemberFunction();

};
