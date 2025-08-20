///////////////////////////////////////////////////////////////////////////////
// This source file is part of the LuaPlus source distribution and is Copyright
// 2001-2012 by Joshua C. Jensen (jjensen@workspacewhiz.com).
//
// The latest version may be obtained from http://luaplus.org/.
//
// The code presented in this file may be used in any environment it is
// acceptable to use Lua.
///////////////////////////////////////////////////////////////////////////////
#ifndef LUAPLUS__LUAPLUSCD_H
#define LUAPLUS__LUAPLUSCD_H

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include <stdlib.h>
#include <string.h>

// LuaPlus Call Dispatcher
namespace LPCD {

	struct LuaLightUserdata {
		LuaLightUserdata(const void* value) :
			m_value(value) {
		}

		const void* m_value;
	};


	struct LuaUserdata {
		LuaUserdata(const void* value) :
			m_value(value) {
		}

		const void* m_value;
	};

	struct LuaNil {
	};

	template <typename, typename = void> struct Type;
	template <typename T> struct Type<      T&> : public Type<T> {};
	template <typename T> struct Type<const T&> : public Type<T> {};

	// bool types
	template<> struct Type<bool> {
		static void Push(lua_State* L, bool value)								{  lua_pushboolean(L, value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TBOOLEAN;  }
		static bool Get(lua_State* L, int idx)									{  return lua_toboolean(L, idx) != 0;  }
	};

	// char types
	template<> struct Type<char> {
		static void Push(lua_State* L, char value)								{  lua_pushinteger(L, value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TNUMBER;  }
		static char Get(lua_State* L, int idx)									{  return static_cast<char>(lua_tonumber(L, idx));  }
	};

	// unsigned char types
	template<> struct Type<unsigned char> {
		static void Push(lua_State* L, unsigned char value)						{  lua_pushinteger(L, value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TNUMBER;  }
		static unsigned char Get(lua_State* L, int idx)							{  return static_cast<unsigned char>(lua_tonumber(L, idx));  }
	};

	// short types
	template<> struct Type<short> {
		static void Push(lua_State* L, short value)								{  lua_pushinteger(L, value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TNUMBER;  }
		static short Get(lua_State* L, int idx)									{  return static_cast<short>(lua_tonumber(L, idx));  }
	};

	// unsigned short types
	template<> struct Type<unsigned short> {
		static void Push(lua_State* L, unsigned short value)						{  lua_pushinteger(L, value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TNUMBER;  }
		static unsigned short Get(lua_State* L, int idx)							{  return static_cast<unsigned short>(lua_tonumber(L, idx));  }
	};

	// int types
	template<> struct Type<int> {
		static void Push(lua_State* L, int value)								{  lua_pushinteger(L, value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TNUMBER;  }
		static int Get(lua_State* L, int idx)									{  return static_cast<int>(lua_tonumber(L, idx));  }
	};

	// unsigned int types
	template<> struct Type<unsigned int> {
		static void Push(lua_State* L, unsigned int value)						{  lua_pushinteger(L, value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TNUMBER;  }
		static unsigned int Get(lua_State* L, int idx)							{  return static_cast<unsigned int>(lua_tonumber(L, idx));  }
	};

#if defined(__APPLE__)
	//// long types
	template<> struct Type<long> {
		static void Push(lua_State* L, long value)								{  lua_pushinteger(L, value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TNUMBER;  }
		static long Get(lua_State* L, int idx)									{  return static_cast<long>(lua_tonumber(L, idx));  }
	};

	//// unsigned long types
	template<> struct Type<unsigned long> {
		static void Push(lua_State* L, unsigned long value)						{  lua_pushinteger(L, value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TNUMBER;  }
		static unsigned long Get(lua_State* L, int idx)							{  return static_cast<unsigned long>(lua_tonumber(L, idx));  }
	};
#endif
    
#if defined(_WIN64)
	// size_t types
	template<> struct Type<size_t> {
		static void Push(lua_State* L, size_t value)								{  lua_pushinteger(L, (lua_Integer)value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TNUMBER;  }
		static size_t Get(lua_State* L, int idx)									{  return static_cast<size_t>(lua_tointeger(L, idx));  }
	};
#endif // _WIN64

	// float types
	template<> struct Type<float> {
		static void Push(lua_State* L, float value)								{  lua_pushnumber(L, (lua_Number)value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TNUMBER;  }
		static float Get(lua_State* L, int idx)									{  return static_cast<float>(lua_tonumber(L, idx));  }
	};

	// double types
	template<> struct Type<double> {
		static void Push(lua_State* L, double value)								{  lua_pushnumber(L, (lua_Number)value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TNUMBER;  }
		static double Get(lua_State* L, int idx)									{  return static_cast<double>(lua_tonumber(L, idx));  }
	};

	// character pointer types
	template<> struct Type<char*> {
		static void Push(lua_State* L, const char* value)						{  lua_pushstring(L, value);  }
		//static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TSTRING;  }
		//static const char* Get(lua_State* L, int idx)							{  return static_cast<const char*>(lua_tostring(L, idx));  }
	};
	template<> struct Type<const char*> : public Type<char*> {};

	// character array types
	template<int NUM_CHARS> struct Type<char [NUM_CHARS]> {
		static void Push(lua_State* L, const char value[NUM_CHARS])				{  lua_pushstring(L, value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TSTRING;  }
		static const char* Get(lua_State* L, int idx)							{  return static_cast<const char*>(lua_tostring(L, idx));  }
	};
	template<int NUM_CHARS> struct Type<const char [NUM_CHARS]> {
		static void Push(lua_State* L, const char value[NUM_CHARS])				{  lua_pushstring(L, value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TSTRING;  }
		static const char* Get(lua_State* L, int idx)							{  return static_cast<const char*>(lua_tostring(L, idx));  }
	};

	// nil type
	template<> struct Type<const LuaNil&> {
		static void Push(lua_State* L, const LuaNil& value)						{  lua_pushnil(L);  }
		static bool Match(lua_State* L, int idx)									{  return true;  }
		static LuaNil Get(lua_State* L, int idx)									{  (void)L, (void)idx;  return LuaNil();  }
	};

	// c function type
	template<> struct Type<lua_CFunction> {
		static void Push(lua_State* L, const lua_CFunction value)				{  lua_pushcclosure(L, value, 0);  }
		static bool Match(lua_State* L, int idx)									{  return true;  }
		static lua_CFunction Get(lua_State* L, int idx)							{  return static_cast<lua_CFunction>(lua_tocfunction(L, idx));  }
	};

	// void types
	template<> struct Type<void*> {
		static void Push(lua_State* L, const void* value)						{  lua_pushlightuserdata(L, (void*)value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TLIGHTUSERDATA;  }
		static void* Get(lua_State* L, int idx)									{  return static_cast<void*>(lua_touserdata(L, idx));  }
	};
	template<> struct Type<const void*> : public Type<void*> {};

	// light userdata type
	template<> struct Type<const LuaLightUserdata&> {
		static void Push(lua_State* L, const LuaLightUserdata& value)			{  lua_pushlightuserdata(L, (void*)value.m_value);  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TLIGHTUSERDATA;  }
		static void* Get(lua_State* L, int idx)									{  return static_cast<void*>(lua_touserdata(L, idx));  }
	};

	// userdata types
	template<> struct Type<LuaUserdata> {
		static void Push(lua_State* L, LuaUserdata value)						{  *(void **)(lua_newuserdata(L, sizeof(void *))) = (void*)value.m_value;  }
		static bool Match(lua_State* L, int idx)									{  return lua_type(L, idx) == LUA_TUSERDATA;  }
		static void* Get(lua_State* L, int idx)									{  return static_cast<void*>(lua_touserdata(L, idx));  }
	};

    void Push(lua_State* L, const char* value, int len);

//	void				Get(TypeWrapper<void>, lua_State*, int)
//		{  }

//	lua_State*		Get(TypeWrapper<lua_State*>, lua_State* L, int /*idx*/)
//		{  return L;  }

//	bool	Match(TypeWrapper<lua_State*>, lua_State* L, int idx)
//		{  return lua_type(L, idx) == LUA_TNONE;  }

	//////////////////////////////////////////////////////////////////////////

	#define luaL_argassert(arg, _index_) if (!Type<P##arg>::Match(L, _index_)) \
				luaL_argerror(L, _index_, "bad argument")

	template<class RT>
	struct ReturnSpecialization {
		static int Call(RT (*func)(), lua_State* L, int /*index*/) {
			RT ret = func();
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename P1>
		static int Call(RT (*func)(P1), lua_State* L, int index) {
			luaL_argassert(1, index + 0);

			RT ret = func(
				Type<P1>::Get(L, index + 0)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename P1, typename P2>
		static int Call(RT (*func)(P1, P2), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			RT ret = func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3>
		static int Call(RT (*func)(P1, P2, P3), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			RT ret = func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3, typename P4>
		static int Call(RT (*func)(P1, P2, P3, P4), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			RT ret = func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3, typename P4,
					typename P5>
		static int Call(RT (*func)(P1, P2, P3, P4, P5), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			RT ret = func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3, typename P4,
					typename P5, typename P6>
		static int Call(RT (*func)(P1, P2, P3, P4, P5, P6), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			RT ret = func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3, typename P4,
					typename P5, typename P6, typename P7>
		static int Call(RT (*func)(P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			RT ret = func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3, typename P4,
					typename P5, typename P6, typename P7, typename P8>
		static int Call(RT (*func)(P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			RT ret = func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3, typename P4,
					typename P5, typename P6, typename P7, typename P8,
					typename P9>
		static int Call(RT (*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			RT ret = func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename P1, typename P2, typename P3, typename P4,
					typename P5, typename P6, typename P7, typename P8,
					typename P9, typename P10>
		static int Call(RT (*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			RT ret = func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8),
				Type<P10>::Get(L, index + 9)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		template <typename Callee>
		static int Call(Callee& callee, RT (Callee::*func)(), lua_State* L, int /*index*/) {
			RT ret = (callee.*func)();
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename Callee>
		static int Call(Callee& callee, RT (Callee::*func)() const, lua_State* L, int /*index*/) {
			RT ret = (callee.*func)();
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename Callee, typename P1>
		static int Call(Callee& callee, RT (Callee::*func)(P1), lua_State* L, int index) {
			luaL_argassert(1, index + 0);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename Callee, typename P1>
		static int Call(Callee& callee, RT (Callee::*func)(P1) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}


		template <typename Callee, typename P1, typename P2>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8,
					typename P9>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8,
					typename P9>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8,
					typename P9, typename P10>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8),
				Type<P10>::Get(L, index + 9)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8,
					typename P9, typename P10>
		static int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			RT ret = (callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8),
				Type<P10>::Get(L, index + 9)
			);
			Type<RT>::Push(L, ret);
			return 1;
		}
	};


	template<>
	struct ReturnSpecialization<void> {
		static int Call(void (*func)(), lua_State* L, int /*index*/) {
			(void)L;
			func();
			return 0;
		}


		template <typename P1>
		static int Call(void (*func)(P1), lua_State* L, int index) {
			luaL_argassert(1, index + 0);

			func(
				Type<P1>::Get(L, index + 0)
			);
			return 0;
		}


		template <typename P1, typename P2>
		static int Call(void (*func)(P1, P2), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1)
			);
			return 0;
		}


		template <typename P1, typename P2, typename P3>
		static int Call(void (*func)(P1, P2, P3), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2)
			);
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4>
		static int Call(void (*func)(P1, P2, P3, P4), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3)
			);
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5>
		static int Call(void (*func)(P1, P2, P3, P4, P5), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4)
			);
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
		static int Call(void (*func)(P1, P2, P3, P4, P5, P6), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5)
			);
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
		static int Call(void (*func)(P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6)
			);
			return 0;
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
		static int Call(void (*func)(P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7)
			);
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
		static int Call(void (*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8)
			);
			return 0;
		}


		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
		static int Call(void (*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			func(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8),
				Type<P10>::Get(L, index + 9)
			);
			return 0;
		}

		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////
		template <typename Callee>
		static int Call(Callee& callee, void (Callee::*func)(), lua_State* /*L*/, int /*index*/) {
			(callee.*func)();
			return 0;
		}


		template <typename Callee>
		static int Call(Callee& callee, void (Callee::*func)() const, lua_State* /*L*/, int /*index*/) {
			(callee.*func)();
			return 0;
		}


		template <typename Callee, typename P1>
		static int Call(Callee& callee, void (Callee::*func)(P1), lua_State* L, int index) {
			luaL_argassert(1, index + 0);

			(callee.*func)(
				Type<P1>::Get(L, index + 0)
			);
			return 0;
		}


		template <typename Callee, typename P1>
		static int Call(Callee& callee, void (Callee::*func)(P1) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);

			(callee.*func)(
				Type<P1>::Get(L, index + 0)
			);
			return 0;
		}


		template <typename Callee, typename P1, typename P2>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6, P7) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8,
					typename P9>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8,
					typename P9>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8,
					typename P9, typename P10>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8),
				Type<P10>::Get(L, index + 9)
			);
			return 0;
		}

		template <typename Callee, typename P1, typename P2, typename P3,
					typename P4, typename P5, typename P6, typename P7, typename P8,
					typename P9, typename P10>
		static int Call(Callee& callee, void (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const, lua_State* L, int index) {
			luaL_argassert(1, index + 0);
			luaL_argassert(2, index + 1);
			luaL_argassert(3, index + 2);
			luaL_argassert(4, index + 3);
			luaL_argassert(5, index + 4);
			luaL_argassert(6, index + 5);
			luaL_argassert(7, index + 6);
			luaL_argassert(8, index + 7);
			luaL_argassert(9, index + 8);
			luaL_argassert(10, index + 9);

			(callee.*func)(
				Type<P1>::Get(L, index + 0),
				Type<P2>::Get(L, index + 1),
				Type<P3>::Get(L, index + 2),
				Type<P4>::Get(L, index + 3),
				Type<P5>::Get(L, index + 4),
				Type<P6>::Get(L, index + 5),
				Type<P7>::Get(L, index + 6),
				Type<P8>::Get(L, index + 7),
				Type<P9>::Get(L, index + 8),
				Type<P10>::Get(L, index + 9)
			);
			return 0;
		}
	};


	template <typename RT>
	int Call(RT (*func)(), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(func, L, index);
	}


	template <typename RT, typename P1>
	int Call(RT (*func)(P1), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(func, L, index);
	}


	template <typename RT, typename P1, typename P2>
	int Call(RT (*func)(P1, P2), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(func, L, index);
	}


	template <typename RT, typename P1, typename P2, typename P3>
	int Call(RT (*func)(P1, P2, P3), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(func, L, index);
	}


	template <typename RT, typename P1, typename P2, typename P3, typename P4>
	int Call(RT (*func)(P1, P2, P3, P4), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(func, L, index);
	}


	template <typename RT, typename P1, typename P2, typename P3, typename P4,
				typename P5>
	int Call(RT (*func)(P1, P2, P3, P4, P5), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(func, L, index);
	}


	template <typename RT, typename P1, typename P2, typename P3, typename P4,
				typename P5, typename P6>
	int Call(RT (*func)(P1, P2, P3, P4, P5, P6), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(func, L, index);
	}


	template <typename RT, typename P1, typename P2, typename P3, typename P4,
				typename P5, typename P6, typename P7>
	int Call(RT (*func)(P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(func, L, index);
	}


	template <typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
	int Call(RT (*func)(P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(func, L, index);
	}


	template <typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
	int Call(RT (*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(func, L, index);
	}


	template <typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
	int Call(RT (*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(func, L, index);
	}


	template <typename Callee, typename RT>
	int Call(Callee& callee, RT (Callee::*func)(), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT>
	int Call(Callee& callee, RT (Callee::*func)() const, lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1>
	int Call(Callee& callee, RT (Callee::*func)(P1), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1>
	int Call(Callee& callee, RT (Callee::*func)(P1) const, lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2) const, lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3) const, lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4) const, lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4, typename P5>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4, typename P5>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5) const, lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4, typename P5, typename P6>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4, typename P5, typename P6>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6) const, lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4, typename P5, typename P6, typename P7>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3,
				typename P4, typename P5, typename P6, typename P7>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7) const, lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8) const, lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const, lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


	template <typename Callee, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
	int Call(Callee& callee, RT (Callee::*func)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const, lua_State* L, int index) {
		return ReturnSpecialization<RT>::Call(callee, func, L, index);
	}


    unsigned char* GetFirstUpvalueAsUserdata(lua_State* L);


	// The Helper class is needed for less fortunate compiler template implementations.
	template <typename Func>
	class DirectCallFunctionDispatchHelper {
	public:
		static int DirectCallFunctionDispatcher(lua_State* L) {
 			unsigned char* buffer = GetFirstUpvalueAsUserdata(L);
			return Call(*(Func*)(buffer), L, 1);
		}
	};


	template <typename Callee, typename Func>
	class DirectCallMemberDispatcherHelper {
	public:
		static int DirectCallMemberDispatcher(lua_State* L) {
 			unsigned char* buffer = GetFirstUpvalueAsUserdata(L);
			return Call(**(Callee**)buffer, *(Func*)(buffer + sizeof(Callee*)), L, 1);
		}
	};

    int lua_StateFunctionDispatcher(lua_State* L);


	template <typename Callee>
	class lua_StateMemberDispatcherHelper {
	public:
		static int lua_StateMemberDispatcher(lua_State* L) {
			typedef int (Callee::*Functor)(lua_State*);
 			unsigned char* buffer = GetFirstUpvalueAsUserdata(L);
			Callee& callee = **(Callee**)buffer;
			Functor& func = *(Functor*)(buffer + sizeof(Callee*));
			return (callee.*func)(L);
		}
	};
} // namespace LPCD


template <typename Func>
void lpcd_pushdirectclosure(lua_State* L, Func func, unsigned int nupvalues = 0) {
	unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(func));
	memcpy(buffer, &func, sizeof(func));
	lua_insert(L, -1 - (int)nupvalues);
	lua_pushcclosure(L, LPCD::DirectCallFunctionDispatchHelper<Func>::DirectCallFunctionDispatcher, nupvalues + 1);
}


template <typename Callee, typename Func>
void lpcd_pushdirectclosure(lua_State* L, const Callee& callee, Func func, unsigned int nupvalues = 0) {
	unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(Callee*) + sizeof(func));
	const void* pCallee = &callee;
	memcpy(buffer, &pCallee, sizeof(Callee*));
	memcpy(buffer + sizeof(Callee*), &func, sizeof(func));
	lua_insert(L, -1 - (int)nupvalues);
	lua_pushcclosure(L, LPCD::DirectCallMemberDispatcherHelper<Callee, Func>::DirectCallMemberDispatcher, nupvalues + 1);
}


void lpcd_pushfunctorclosure(lua_State* L, int(*func)(lua_State*), unsigned int nupvalues = 0);


template <typename Callee>
void lpcd_pushfunctorclosureex(lua_State* L, const Callee& callee, int (Callee::*func)(lua_State*), unsigned int nupvalues = 0) {
	unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(Callee*) + sizeof(func));
	const void* pCallee = &callee;
	memcpy(buffer, &pCallee, sizeof(Callee*));
	memcpy(buffer + sizeof(Callee*), &func, sizeof(func));
	lua_insert(L, -1 - (int)nupvalues);
	lua_pushcclosure(L, LPCD::lua_StateMemberDispatcherHelper<Callee>::lua_StateMemberDispatcher, nupvalues + 1);
}


///////////////////////////////////////////////////////////////////////////////

namespace LPCD {
    void* GetObjectUserdata(lua_State* L);
    void* GetInPlaceObjectUserdata(lua_State* L);

	template <typename Callee>
	class Object_MemberDispatcherHelper {
	public:
		static int Object_MemberDispatcher(lua_State* L) {
			typedef int (Callee::*Functor)(lua_State*);
 			unsigned char* buffer = GetFirstUpvalueAsUserdata(L);
			Functor& func = *(Functor*)(buffer);
			Callee& callee = *(Callee*)GetObjectUserdata(L);
			return (callee.*func)(L);
		}
	};

	template <typename Callee, typename Func, int startIndex>
	class DirectCallObjectMemberDispatcherHelper {
	public:
		static int DirectCallMemberDispatcher(lua_State* L) {
 			unsigned char* buffer = GetFirstUpvalueAsUserdata(L);
			Callee& callee = *(Callee*)GetObjectUserdata(L);
			return Call(callee, *(Func*)buffer, L, startIndex);
		}
	};

	template <typename Callee, typename Func, int startIndex>
	class DirectCallInPlaceObjectMemberDispatcherHelper {
	public:
		static int DirectCallMemberDispatcher(lua_State* L) {
 			unsigned char* buffer = GetFirstUpvalueAsUserdata(L);
			Callee& callee = *(Callee*)GetInPlaceObjectUserdata(L);
			return Call(callee, *(Func*)buffer, L, startIndex);
		}
	};

    int PropertyMetatable_newindex(lua_State* L);

	// function gettable_event (table, key)
	// upvalues:
	//     1 - function__index
	//     2 - properties__index
    int PropertyMetatable_index(lua_State* L);

	template <typename Object, typename VarType>
	class PropertyMemberHelper {
	public:
		static int PropertyGet(lua_State* L) {
			Object* obj = (Object*)LPCD::GetObjectUserdata(L);
			void* offset = lua_touserdata(L, 3);
			LPCD::Type<VarType>::Push(L, *(VarType*)((unsigned char*)obj + (ptrdiff_t)offset));
			return 1;
		}

		static int PropertySet(lua_State* L) {
			Object* obj = (Object*)LPCD::GetObjectUserdata(L);
			void* offset = lua_touserdata(L, 4);

			if (!LPCD::Type<VarType>::Match(L, 3))
				luaL_argerror(L, 3, "bad argument");

			*(VarType*)((unsigned char*)obj + (ptrdiff_t)offset) = LPCD::Type<VarType>::Get(L, 3);
			return 0;
		}
	};

	template <typename Object, typename VarType>
	class InPlacePropertyMemberHelper {
	public:
		static int PropertyGet(lua_State* L) {
			Object* obj = (Object*)LPCD::GetInPlaceObjectUserdata(L);
			void* offset = lua_touserdata(L, 3);
			LPCD::Type<VarType>::Push(L, *(VarType*)((unsigned char*)obj + (ptrdiff_t)offset));
			return 1;
		}

		static int PropertySet(lua_State* L) {
			Object* obj = (Object*)LPCD::GetInPlaceObjectUserdata(L);
			void* offset = lua_touserdata(L, 4);

			if (!Type<VarType>::Match(L, 3))
				luaL_argerror(L, 3, "bad argument");

			*(VarType*)((unsigned char*)obj + (ptrdiff_t)offset) = Type<VarType>::Get(L, 3);
			return 0;
		}
	};

	template <typename VarType>
	class PropertyGlobalHelper {
	public:
		static int PropertyGet(lua_State* L) {
			void* offset = lua_touserdata(L, lua_upvalueindex(1));

			LPCD::Type<VarType>::Push(L, *(VarType*)offset);

			return 1;
		}

		static int PropertySet(lua_State* L) {
			void* offset = lua_touserdata(L, lua_upvalueindex(1));

			if (!LPCD::Type<VarType>::Match(L, 1))
				luaL_argerror(L, 2, "bad argument");

			*(VarType*)offset = LPCD::Type<VarType>::Get(L, 1);

			return 1;
		}
	};
} // namespace LPCD


template <typename Callee>
void lpcd_pushobjectfunctorclosure(lua_State* L, int (Callee::*func)(lua_State*), unsigned int nupvalues = 0) {
	unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(func));
	memcpy(buffer, &func, sizeof(func));
	lua_pushcclosure(L, LPCD::Object_MemberDispatcherHelper<Callee>::Object_MemberDispatcher, nupvalues + 1);
}


template <typename Callee, typename Func>
void lpcd_pushobjectdirectclosure(lua_State* L, const Callee* callee, Func func, unsigned int nupvalues = 0) {
	unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(func));
	memcpy(buffer, &func, sizeof(func));
	lua_pushcclosure(L, LPCD::DirectCallObjectMemberDispatcherHelper<Callee, Func, 2>::DirectCallMemberDispatcher, nupvalues + 1);
}


template <typename Callee>
void lpcd_pushinplaceobjectfunctorclosure(lua_State* L, int (Callee::*func)(lua_State*), unsigned int nupvalues = 0) {
	unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(func));
	memcpy(buffer, &func, sizeof(func));
	//lua_pushcclosure(L, LPCD::Object_InPlaceMemberDispatcherHelper<Callee>::Object_MemberDispatcher, nupvalues + 1);
}


template <typename Callee, typename Func>
void lpcd_pushinplaceobjectdirectclosure(lua_State* L, const Callee* callee, Func func, unsigned int nupvalues = 0) {
	unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(func));
	memcpy(buffer, &func, sizeof(func));
	lua_pushcclosure(L, LPCD::DirectCallInPlaceObjectMemberDispatcherHelper<Callee, Func, 2>::DirectCallMemberDispatcher, nupvalues + 1);
}


template <typename Object, typename VarType>
void lpcd_pushmemberpropertygetclosure(lua_State* L, VarType Object::* var) {
	lua_pushlightuserdata(L, (void*)&(((Object*)0)->*var));
	lua_pushlightuserdata(L, (void*)-2);
	lua_pushcclosure(L, &LPCD::PropertyMemberHelper<Object, VarType>::PropertyGet, 2);
}


template <typename Object, typename VarType>
void lpcd_pushmemberpropertysetclosure(lua_State* L, VarType Object::* var) {
	lua_pushlightuserdata(L, (void*)&(((Object*)0)->*var));
	lua_pushlightuserdata(L, (void*)-2);
	lua_pushcclosure(L, &LPCD::PropertyMemberHelper<Object, VarType>::PropertySet, 2);
}


template <typename Object, typename VarType>
void lpcd_pushmemberinplacepropertygetclosure(lua_State* L, VarType Object::* var) {
	lua_pushlightuserdata(L, (void*)&(((Object*)0)->*var));
	lua_pushlightuserdata(L, (void*)-2);
	lua_pushcclosure(L, &LPCD::InPlacePropertyMemberHelper<Object, VarType>::PropertyGet, 2);
}


template <typename Object, typename VarType>
void lpcd_pushmemberinplacepropertysetclosure(lua_State* L, VarType Object::* var) {
	lua_pushlightuserdata(L, (void*)&(((Object*)0)->*var));
	lua_pushlightuserdata(L, (void*)-2);
	lua_pushcclosure(L, &LPCD::InPlacePropertyMemberHelper<Object, VarType>::PropertySet, 2);
}


template <typename VarType>
void lpcd_pushglobalpropertygetclosure(lua_State* L, VarType* var) {
	lua_pushlightuserdata(L, (void*)var);
	lua_pushlightuserdata(L, (void*)-2);
	lua_pushcclosure(L, &LPCD::PropertyGlobalHelper<VarType>::PropertyGet, 2);
}


template <typename VarType>
void lpcd_pushglobalpropertysetclosure(lua_State* L, VarType* var) {
	lua_pushlightuserdata(L, (void*)var);
	lua_pushlightuserdata(L, (void*)-2);
	lua_pushcclosure(L, &LPCD::PropertyGlobalHelper<VarType>::PropertySet, 2);
}


/* convert a stack index to positive */
#define lpcd_abs_index(L, i)		((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)

void lpcd_integratepropertysupport(lua_State* L, int metatableIndex, bool inPlace = false);
void lpcd_propertymetatable_getfunctions(lua_State* L, int metatableIndex);
void lpcd_newclassmetatable(lua_State* L, const char* className, const char* baseClassName, bool inPlace = false);

template <typename Object, typename VarType>
void lpcd_propertycreate(lua_State* L, int metatableIndex, const char* varName, VarType Object::* var, bool read = true, bool write = true) {
	lua_pushvalue(L, metatableIndex);				// metatable

	lua_pushstring(L, varName);						// metatable varName

	if (read) {
		lua_getfield(L, -2, "__index");				// metatable varName __index
		lua_getupvalue(L, -1, 1);					// metatable varName __index functions
		lua_getupvalue(L, -2, 2);					// metatable varName __index functions inPlace
		bool inPlace = lua_toboolean(L, -1) != 0;
		lua_pop(L, 1);								// metatable varName __index functions
		lua_pushvalue(L, -3);						// metatable varName __index functions varName
		inPlace ? lpcd_pushmemberinplacepropertygetclosure(L, var) : lpcd_pushmemberpropertygetclosure(L, var);
													// metatable varName __index functions varName closure
		lua_rawset(L, -3);							// metatable varName __index functions
		lua_pop(L, 2);								// metatable varName
	}

	if (write) {
		lua_getfield(L, -2, "__newindex");			// metatable varName __newindex
		lua_getupvalue(L, -1, 1);					// metatable varName __newindex functions
		lua_getupvalue(L, -2, 2);					// metatable varName __newindex functions inPlace
		bool inPlace = lua_toboolean(L, -1) != 0;
		lua_pop(L, 1);								// metatable varName __newindex functions
		lua_pushvalue(L, -3);						// metatable varName __newindex functions varName
		inPlace ? lpcd_pushmemberinplacepropertysetclosure(L, var) : lpcd_pushmemberpropertysetclosure(L, var);
													// metatable varName __newindex functions varName closure
		lua_rawset(L, -3);							// metatable varName __newindex functions
		lua_pop(L, 2);								// metatable varName
	}

	lua_pop(L, 2);
}


void* lpcd_checkobject(lua_State* L, int index, const char* tname, bool throwError = true);

namespace LPCD {

class Class {
public:
	Class(lua_State* L, const char* className, const char* baseClassName = NULL)
		: L(L)
	{
		lpcd_newclassmetatable(L, className, baseClassName);
		lpcd_propertymetatable_getfunctions(L, -1);
	}

	~Class() {
		lua_pop(L, 2);
	}

	void MetatableFunction(const char* name, lua_CFunction func, unsigned int nupvalues = 0) {
		lua_pushcclosure(L, func, nupvalues);
		lua_setfield(L, -3, name);
	}

	template <typename Object, typename VarType>
	Class& Property(const char* varName, VarType Object::* var, bool read = true, bool write = true) {
		lpcd_propertycreate(L, -2, varName, var, read, write);
		return *this;
	}

	void Function(const char* name, lua_CFunction func, unsigned int nupvalues = 0) {
		lua_pushcclosure(L, func, nupvalues);
		lua_setfield(L, -2, name);
	}

	template <typename Callee>
	Class& ObjectFunctor(const char* name, int (Callee::*func)(lua_State*), unsigned int nupvalues = 0) {
		lpcd_pushobjectfunctorclosure(L, func, nupvalues);
		lua_setfield(L, -2, name);
		return *this;
	}

	template <typename Callee, typename Func>
	Class& ObjectDirect(const char* name, const Callee* callee, Func func, unsigned int nupvalues = 0) {
		lpcd_pushobjectdirectclosure(L, callee, func, nupvalues);
		lua_setfield(L, -2, name);
		return *this;
	}

	lua_State* L;
};


class InPlaceClass {
public:
	InPlaceClass(lua_State* L, const char* className, const char* baseClassName = NULL)
		: L(L)
	{
		lpcd_newclassmetatable(L, className, baseClassName, true);
		lpcd_propertymetatable_getfunctions(L, -1);
	}

	~InPlaceClass() {
		lua_pop(L, 2);
	}

	void MetatableFunction(const char* name, lua_CFunction func, unsigned int nupvalues = 0) {
		lua_pushcclosure(L, func, nupvalues);
		lua_setfield(L, -3, name);
	}

	template <typename Object, typename VarType>
	InPlaceClass& Property(const char* varName, VarType Object::* var, bool read = true, bool write = true) {
		lpcd_propertycreate(L, -2, varName, var, read, write);
		return *this;
	}

	template <typename Callee>
	InPlaceClass& ObjectFunctor(const char* name, int (Callee::*func)(lua_State*), unsigned int nupvalues = 0) {
		lpcd_pushinplaceobjectfunctorclosure(L, func, nupvalues);
		lua_setfield(L, -2, name);
		return *this;
	}

	template <typename Callee, typename Func>
	InPlaceClass& ObjectDirect(const char* name, const Callee* callee, Func func, unsigned int nupvalues = 0) {
		lpcd_pushinplaceobjectdirectclosure(L, callee, func, nupvalues);
		lua_setfield(L, -2, name);
		return *this;
	}

	lua_State* L;
};

} // namespace LPCD

#endif // LUAPLUS__LUAPLUSCD_H
