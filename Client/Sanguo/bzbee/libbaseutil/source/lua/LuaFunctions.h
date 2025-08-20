/*!
* \file LuaFunctions.h
*
* \author zhichenxia
* \date 2017/12/08
*
*
*/
#include "ScriptVM.h"
#include "LuaPlus/LuaPlus.h"


namespace LPCD
{
	// glm::vec3
	template<> struct Type<glm::vec3>
	{
		static void Push(lua_State* L, glm::vec3 value)
		{
			lua_createtable(L, 0, 3);
			auto table = lua_gettop(L);

			lua_getglobal(L, "vec3");
			lua_setmetatable(L, table);

			lua_pushnumber(L, (lua_Number)value.x);
			lua_setfield(L, table, "x");

			lua_pushnumber(L, (lua_Number)value.y);
			lua_setfield(L, table, "y");

			lua_pushnumber(L, (lua_Number)value.z);
			lua_setfield(L, table, "z");
		}

		static bool Match(lua_State* L, int idx)
		{
			int b = lua_istable(L, idx);
			if (!b) return false;

			lua_getfield(L, idx, "x");
			b = lua_isnumber(L, -1);
			lua_pop(L, 1);
			if (!b) return false;

			lua_getfield(L, idx, "y");
			b = lua_isnumber(L, -1);
			lua_pop(L, 1);
			if (!b) return false;

			lua_getfield(L, idx, "z");
			b = lua_isnumber(L, -1);
			lua_pop(L, 1);
			if (!b) return false;

			return true;
		}

		static glm::vec3 Get(lua_State* L, int idx)
		{
			lua_getfield(L, idx, "x");
			float x = static_cast<float>(lua_tonumber(L, -1));
			lua_pop(L, 1);

			lua_getfield(L, idx, "y");
			float y = static_cast<float>(lua_tonumber(L, -1));
			lua_pop(L, 1);

			lua_getfield(L, idx, "z");
			float z = static_cast<float>(lua_tonumber(L, -1));
			lua_pop(L, 1);

			return glm::vec3(x, y, z);
		}
	};
	template<> struct Type<      glm::vec3&> : public Type<glm::vec3>{};
	template<> struct Type<const glm::vec3&> : public Type<glm::vec3>{};
}


#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) PP_CAT_II(~, a ## b)
#define PP_CAT_II(p, res) res


#define LUA_IMPL(...) (__VA_ARGS__); } static PP_CAT(__lua_reg_func_, __LINE__); decltype(PP_CAT(__lua_retval_, __LINE__)()) PP_CAT(__LuaRegFunc_, __LINE__)::func(__VA_ARGS__)
#define LUA(name) PP_CAT(__lua_retval_, __LINE__)(void); \
	class PP_CAT(__LuaRegFunc_, name); typedef PP_CAT(__LuaRegFunc_, name) PP_CAT(__LuaRegFunc_, __LINE__); class PP_CAT(__LuaRegFunc_, name) { \
	public: PP_CAT(__LuaRegFunc_, name)() { ScriptVM::registerFunction([] { ScriptVM::getInstance().getState()->GetGlobals().RegisterDirect(#name, func); }); } \
	private: static decltype(PP_CAT(__lua_retval_, __LINE__)()) func LUA_IMPL
