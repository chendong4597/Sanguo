///////////////////////////////////////////////////////////////////////////////
// This source file is part of the LuaPlus source distribution and is Copyright
// 2001-2011 by Joshua C. Jensen (jjensen@workspacewhiz.com).
//
// The latest version may be obtained from http://luaplus.org/.
//
// The code presented in this file may be used in any environment it is
// acceptable to use Lua.
///////////////////////////////////////////////////////////////////////////////
#ifndef LUAPLUS__LUACALL_H
#define LUAPLUS__LUACALL_H

#include "LuaPlusInternal.h"
#include "LuaStackObject.h"
#include "LuaObject.h"

namespace LuaPlus {

struct LuaRun
{
    LuaRun(int _numResults = -1, int _alertStackPos = 0) throw() :
        numResults(_numResults),
        alertStackPos(_alertStackPos) {
	}

	int numResults;
    int alertStackPos;
};


class LuaCall {
public:
	LuaCall(LuaObject& functionObj);

	LuaStackObject operator<<(const LuaRun& /*run*/);
	LuaCall& operator=(const LuaCall& src);

	lua_State* L;
	int numArgs;
	int startResults;
};

LuaCall& operator<<(LuaCall& call, const LuaArgNil& value);
LuaCall& operator<<(LuaCall& call, float value);
LuaCall& operator<<(LuaCall& call, double value);
LuaCall& operator<<(LuaCall& call, int value);
LuaCall& operator<<(LuaCall& call, unsigned int value);
LuaCall& operator<<(LuaCall& call, const char* value);
LuaCall& operator<<(LuaCall& call, lua_CFunction value);
LuaCall& operator<<(LuaCall& call, int (*value)(LuaState*));
LuaCall& operator<<(LuaCall& call, bool value);
LuaCall& operator<<(LuaCall& call, void* value);
LuaCall& operator<<(LuaCall& call, LuaStackObject& value);
LuaCall& operator<<(LuaCall& call, LuaObject& value);

} // namespace LuaPlus

#endif // LUAPLUS__LUACALL_H
