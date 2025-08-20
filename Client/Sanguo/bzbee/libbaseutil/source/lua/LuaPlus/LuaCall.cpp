///////////////////////////////////////////////////////////////////////////////
// This source file is part of the LuaPlus source distribution and is Copyright
// 2001-2011 by Joshua C. Jensen (jjensen@workspacewhiz.com).
//
// The latest version may be obtained from http://luaplus.org/.
//
// The code presented in this file may be used in any environment it is
// acceptable to use Lua.
///////////////////////////////////////////////////////////////////////////////
#include "LuaCall.h"
#include "LuaState.h"

///////////////////////////////////////////////////////////////////////////////
// namespace LuaPlus
///////////////////////////////////////////////////////////////////////////////
namespace LuaPlus
{

LuaCall::LuaCall(LuaObject& functionObj)
	: L(functionObj.GetCState())
	, numArgs(0) {
	luaplus_assert(functionObj.IsFunction());
	startResults = lua_gettop(L) + 1;
	functionObj.Push(L);
}

LuaCall& operator<<(LuaCall& call, const LuaArgNil& /*value*/) {
	lua_pushnil(call.L);
	++call.numArgs;
	return call;
}

LuaCall& operator<<(LuaCall& call, float value) {
	lua_pushnumber(call.L, value);
	++call.numArgs;
	return call;
}

LuaCall& operator<<(LuaCall& call, double value) {
	lua_pushnumber(call.L, (lua_Number)value);
	++call.numArgs;
	return call;
}

LuaCall& operator<<(LuaCall& call, int value) {
	lua_pushinteger(call.L, value);
	++call.numArgs;
	return call;
}

LuaCall& operator<<(LuaCall& call, unsigned int value) {
	lua_pushinteger(call.L, value);
	++call.numArgs;
	return call;
}

LuaCall& operator<<(LuaCall& call, const char* value) {
	lua_pushstring(call.L, value);
	++call.numArgs;
	return call;
}

LuaCall& operator<<(LuaCall& call, lua_CFunction value) {
	lua_pushcclosure(call.L, value, 0);
	++call.numArgs;
	return call;
}

LuaCall& operator<<(LuaCall& call, int (*value)(LuaState*)) {
	LuaState* state = lua_State_to_LuaState(call.L);
	state->PushCClosure(value, 0);
	++call.numArgs;
	return call;
}

LuaCall& operator<<(LuaCall& call, bool value) {
	lua_pushboolean(call.L, value);
	++call.numArgs;
	return call;
}

LuaCall& operator<<(LuaCall& call, void* value) {
	lua_pushlightuserdata(call.L, value);
	++call.numArgs;
	return call;
}

LuaCall& operator<<(LuaCall& call, LuaStackObject& value) {
	luaplus_assert(call.L == value.GetCState());
	value.Push();
	++call.numArgs;
	return call;
}

LuaCall& operator<<(LuaCall& call, LuaObject& value) {
	luaplus_assert(call.L == value.GetCState());
	value.Push(call.L);
	++call.numArgs;
	return call;
}

LuaStackObject LuaCall::operator<<(const LuaRun& run) {
//	LuaState* state = lua_State_to_LuaState(L);
    int resultsStackPos = lua_gettop(L) - numArgs;
	int err = lua_pcall(L, numArgs, run.numResults, run.alertStackPos);
	if (err != 0) {
        if (lua_isstring(L, -1)) {
			const char *errStr = lua_tostring(L, -1);
            // Does this string persist long enough?
            luaplus_throw(errStr);
        } else {
            luaplus_throw(fmt::format("unknown lua error, code: {0}.", err).c_str());
        }
	}
	return LuaStackObject(L, resultsStackPos);
}

LuaCall& LuaCall::operator=(const LuaCall& src) {
	L = src.L;
	numArgs = src.numArgs;
	startResults = src.startResults;

	return *this;
}

} // namespace LuaPlus
