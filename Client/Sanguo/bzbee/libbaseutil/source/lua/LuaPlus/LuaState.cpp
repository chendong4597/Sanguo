///////////////////////////////////////////////////////////////////////////////
// This source file is part of the LuaPlus source distribution and is Copyright
// 2001-2011 by Joshua C. Jensen (jjensen@workspacewhiz.com).
//
// The latest version may be obtained from http://luaplus.org/.
//
// The code presented in this file may be used in any environment it is
// acceptable to use Lua.
///////////////////////////////////////////////////////////////////////////////
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "LuaStateCD.h"
#include "LuaStackObject.h"
#include "LuaState.h"
#include "LuaObject.h"

///////////////////////////////////////////////////////////////////////////////
// namespace LuaPlus
///////////////////////////////////////////////////////////////////////////////
namespace LuaPlus
{

LuaState* LuaState::Create()
{
	return lua_State_to_LuaState(luaL_newstate());
}


LuaState* LuaState::Create(bool initStandardLibrary)
{
	LuaState* state = lua_State_to_LuaState(luaL_newstate());
	if (initStandardLibrary)
		state->OpenLibs();
	return state;
}


LuaState* LuaState::Create(lua_Alloc allocFunction, void* userdata)
{
	return lua_State_to_LuaState(lua_newstate(allocFunction, userdata));
}


LuaState* LuaState::Create(lua_Alloc allocFunction, void* userdata, bool initStandardLibrary)
{
	LuaState* state = lua_State_to_LuaState(lua_newstate(allocFunction, userdata));
	if (initStandardLibrary)
		state->OpenLibs();
	return state;
}


LuaObject LuaState::CreateThread(LuaState* parentState)
{
	lua_State* L = LuaState_to_lua_State(parentState);
    lua_State* L1 = lua_newthread(L);
	lua_xmove(L, L1, 1);
	LuaObject ret(L1, -1);
	lua_pop(L1, 1);
	return ret;
}


void LuaState::Destroy(LuaState* state) {
	lua_State* L = LuaState_to_lua_State(state);
	lua_close(L);
}


lua_CFunction LuaState::AtPanic(lua_CFunction panicf)
{
	return lua_atpanic(LuaState_to_lua_State(this), panicf);
}

LuaStackObject LuaState::Stack(int index)
{
    return LuaStackObject(this, index);
}

LuaStackObject LuaState::StackTop()
{
    return LuaStackObject(this, GetTop());
}

// Basic stack manipulation.
int LuaState::AbsIndex(int index)
{
#if LUA_VERSION_NUM == 501
	assert(0);
	return 0;
#elif LUA_VERSION_NUM >= 502
	return lua_absindex(LuaState_to_lua_State(this), index);
#endif
}

int LuaState::GetTop()
{
	return lua_gettop(LuaState_to_lua_State(this));
}

void LuaState::SetTop(int index)
{
	lua_settop(LuaState_to_lua_State(this), index);
}

void LuaState::PushGlobalTable() {
#if LUA_VERSION_NUM == 501
	lua_pushvalue(LuaState_to_lua_State(this), LUA_GLOBALSINDEX);
#elif LUA_VERSION_NUM >= 502
	lua_pushglobaltable(LuaState_to_lua_State(this));
#endif
}

void LuaState::PushValue(int index)
{
	lua_pushvalue(LuaState_to_lua_State(this), index);
}

void LuaState::PushValue(LuaStackObject& object)
{
	lua_pushvalue(LuaState_to_lua_State(this), object);
}

#if LUA_VERSION_NUM >= 503
void LuaState::Rotate(int index, int n)
{
	lua_rotate(LuaState_to_lua_State(this), index, n);
}
#endif

void LuaState::Remove(int index)
{
	lua_remove(LuaState_to_lua_State(this), index);
}

void LuaState::Insert(int index)
{
	lua_insert(LuaState_to_lua_State(this), index);
}

void LuaState::Replace(int index)
{
	lua_replace(LuaState_to_lua_State(this), index);
}

void LuaState::Copy(int fromindex, int toindex)
{
#if LUA_VERSION_NUM == 501
	assert(0);
#elif LUA_VERSION_NUM >= 502
	lua_copy(LuaState_to_lua_State(this), fromindex, toindex);
#endif
}


int LuaState::CheckStack(int size)
{
	return lua_checkstack(LuaState_to_lua_State(this), size);
}


void LuaState::XMove(LuaState* to, int n)
{
	lua_xmove(LuaState_to_lua_State(this), LuaState_to_lua_State(to), n);
}


// access functions (stack -> C)
int LuaState::IsNumber(int index) const {
	return lua_isnumber(LuaState_to_lua_State(this), index);
}


int LuaState::IsString(int index) const {
	return lua_isstring(LuaState_to_lua_State(this), index);
}


int LuaState::IsCFunction(int index) const {
	return lua_iscfunction(LuaState_to_lua_State(this), index);
}


int LuaState::IsInteger(int index) const {
#if LUA_VERSION_NUM >= 503
	return lua_isinteger(LuaState_to_lua_State(this), index);
#else
	return lua_isnumber(LuaState_to_lua_State(this), index);
#endif
}


int LuaState::IsUserdata(int index) const {
	return lua_isuserdata(LuaState_to_lua_State(this), index);
}


int LuaState::IsFunction(int index) const {
	return lua_isfunction(LuaState_to_lua_State(this), index);
}


int LuaState::IsTable(int index) const {
	return lua_istable(LuaState_to_lua_State(this), index);
}


int LuaState::IsLightUserdata(int index) const {
	return lua_islightuserdata(LuaState_to_lua_State(this), index);
}


int LuaState::IsNil(int index) const {
	return lua_isnil(LuaState_to_lua_State(this), index);
}


int LuaState::IsBoolean(int index) const {
	return lua_isboolean(LuaState_to_lua_State(this), index);
}


int LuaState::IsThread(int index) const {
	return lua_isthread(LuaState_to_lua_State(this), index);
}


int LuaState::IsNone(int index) const {
	return lua_isnone(LuaState_to_lua_State(this), index);
}


int LuaState::IsNoneOrNil(int index) const {
	return lua_isnoneornil(LuaState_to_lua_State(this), index);
}


int LuaState::Type(int index) const {
	return lua_type(LuaState_to_lua_State(this), index);
}


const char* LuaState::TypeName(int type) {
	return lua_typename(LuaState_to_lua_State(this), type);
}


void LuaState::Arith(int op)
{
#if LUA_VERSION_NUM == 501
	assert(0);
#elif LUA_VERSION_NUM >= 502
	return lua_arith(LuaState_to_lua_State(this), op);
#endif
}

int LuaState::Equal(int index1, int index2)
{
#if LUA_VERSION_NUM == 501
	return lua_equal(LuaState_to_lua_State(this), index1, index2);
#elif LUA_VERSION_NUM >= 502
	return lua_compare(LuaState_to_lua_State(this), index1, index2, LUA_OPEQ);
#endif
}

int LuaState::RawEqual(int index1, int index2)
{
	return lua_rawequal(LuaState_to_lua_State(this), index1, index2);
}

int LuaState::Compare(int index1, int index2, int op)
{
#if LUA_VERSION_NUM == 501
	assert(0);
	return 0;
#elif LUA_VERSION_NUM >= 502
	return lua_compare(LuaState_to_lua_State(this), index1, index2, op);
#endif
}

int LuaState::LessThan(int index1, int index2)
{
#if LUA_VERSION_NUM == 501
	return lua_lessthan(LuaState_to_lua_State(this), index1, index2);
#elif LUA_VERSION_NUM >= 502
	return lua_compare(LuaState_to_lua_State(this), index1, index2, LUA_OPLT);
#endif
}


lua_Number LuaState::ToNumber(int index) {
	return lua_tonumber(LuaState_to_lua_State(this), index);
}


lua_Number LuaState::ToNumberX(int index, int *isnum) {
#if LUA_VERSION_NUM == 501
	if (isnum)
		*isnum = 1;
	return lua_tonumber(LuaState_to_lua_State(this), index);
#elif LUA_VERSION_NUM >= 502
	return lua_tonumberx(LuaState_to_lua_State(this), index, isnum);
#endif
}


lua_Integer LuaState::ToInteger(int index) {
	return lua_tointeger(LuaState_to_lua_State(this), index);
}


lua_Integer LuaState::ToIntegerX(int index, int *isnum) {
#if LUA_VERSION_NUM == 501
	if (isnum)
		*isnum = 1;
	return lua_tointeger(LuaState_to_lua_State(this), index);
#elif LUA_VERSION_NUM >= 502
	return lua_tointegerx(LuaState_to_lua_State(this), index, isnum);
#endif
}


#if LUA_VERSION_NUM == 501

unsigned int LuaState::ToUnsigned(int index) {
	return (unsigned int)lua_tointeger(LuaState_to_lua_State(this), index);
}


unsigned int LuaState::ToUnsignedX(int index, int *isnum) {
	if (isnum)
		*isnum = 1;
	return (unsigned int)lua_tointeger(LuaState_to_lua_State(this), index);
}

#elif LUA_VERSION_NUM == 502

lua_Unsigned LuaState::ToUnsigned(int index) {
	return lua_tounsigned(LuaState_to_lua_State(this), index);
}


lua_Unsigned LuaState::ToUnsignedX(int index, int *isnum) {
	return lua_tounsignedx(LuaState_to_lua_State(this), index, isnum);
}

#elif LUA_VERSION_NUM == 503

lua_Unsigned LuaState::ToUnsigned(int index) {
	return lua_tointegerx(LuaState_to_lua_State(this), index, NULL);
}


lua_Unsigned LuaState::ToUnsignedX(int index, int *isnum) {
	return lua_tointegerx(LuaState_to_lua_State(this), index, isnum);
}

#endif


int LuaState::ToBoolean(int index) {
	return lua_toboolean(LuaState_to_lua_State(this), index);
}


const char* LuaState::ToLString(int index, size_t* len) {
	return lua_tolstring(LuaState_to_lua_State(this), index, len);
}


const char* LuaState::ToString(int index) {
	return lua_tostring(LuaState_to_lua_State(this), index);
}


size_t LuaState::RawLen(int index) {
#if LUA_VERSION_NUM == 501
	return lua_objlen(LuaState_to_lua_State(this), index);
#elif LUA_VERSION_NUM >= 502
	return lua_rawlen(LuaState_to_lua_State(this), index);
#endif
}


size_t LuaState::ObjLen(int index) {
#if LUA_VERSION_NUM == 501
	return lua_objlen(LuaState_to_lua_State(this), index);
#elif LUA_VERSION_NUM >= 502
	return lua_rawlen(LuaState_to_lua_State(this), index);
#endif
}


lua_CFunction LuaState::ToCFunction(int index) {
	return lua_tocfunction(LuaState_to_lua_State(this), index);
}


void* LuaState::ToUserdata(int index) {
	return lua_touserdata(LuaState_to_lua_State(this), index);
}


lua_State* LuaState::ToThread(int index) {
	return lua_tothread(LuaState_to_lua_State(this), index);
}


const void* LuaState::ToPointer(int index) {
	return lua_topointer(LuaState_to_lua_State(this), index);
}

	
int LuaState::Equal(const LuaObject& o1, const LuaObject& o2) {
#if LUA_VERSION_NUM == 501
#if LUA_FASTREF_SUPPORT
	return lua_equal(o1.L, o1.ref, o2.ref);
#else
	LuaFastRefPush _frp1(&o1);
	LuaFastRefPush _frp2(&o2);
	return lua_equal(o1.L, -2, -1);
#endif // LUA_FASTREF_SUPPORT
#elif LUA_VERSION_NUM >= 502
#if LUA_FASTREF_SUPPORT
	return lua_compare(o1.L, o1.ref, o2.ref, LUA_OPEQ);
#else
	LuaFastRefPush _frp1(&o1);
	LuaFastRefPush _frp2(&o2);
	return lua_compare(o1.L, -2, -1, LUA_OPEQ);
#endif // LUA_FASTREF_SUPPORT
#endif
}


int LuaState::LessThan(const LuaObject& o1, const LuaObject& o2) {
#if LUA_VERSION_NUM == 501
#if LUA_FASTREF_SUPPORT
	return lua_lessthan(o1.L, o1.ref, o2.ref);
#else
	LuaFastRefPush _frp1(&o1);
	LuaFastRefPush _frp2(&o2);
	return lua_lessthan(o1.L, -2, -1);
#endif // LUA_FASTREF_SUPPORT
#elif LUA_VERSION_NUM >= 502
#if LUA_FASTREF_SUPPORT
	return lua_compare(o1.L, o1.ref, o2.ref, LUA_OPLT);
#else
	LuaFastRefPush _frp1(&o1);
	LuaFastRefPush _frp2(&o2);
	return lua_compare(o1.L, -2, -1, LUA_OPLT);
#endif // LUA_FASTREF_SUPPORT
#endif
}


// push functions (C -> stack)
LuaStackObject LuaState::PushNil()
{
	lua_pushnil(LuaState_to_lua_State(this));
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

LuaStackObject LuaState::PushNumber(lua_Number n)
{
	lua_pushnumber(LuaState_to_lua_State(this), n);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

LuaStackObject LuaState::PushInteger(lua_Integer n)
{
	lua_pushinteger(LuaState_to_lua_State(this), n);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

#if LUA_VERSION_NUM == 501

LuaStackObject LuaState::PushUnsigned(unsigned int n)
{
	lua_pushnumber(LuaState_to_lua_State(this), (unsigned int)n);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

#elif LUA_VERSION_NUM == 502

LuaStackObject LuaState::PushUnsigned(lua_Unsigned n)
{
	lua_pushunsigned(LuaState_to_lua_State(this), n);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

#elif LUA_VERSION_NUM >= 503

LuaStackObject LuaState::PushUnsigned(lua_Unsigned n)
{
	lua_pushinteger(LuaState_to_lua_State(this), n);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

#endif

LuaStackObject LuaState::PushLString(const char *s, size_t len)
{
	lua_pushlstring(LuaState_to_lua_State(this), s, len);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

LuaStackObject LuaState::PushString(const char *s)
{
	lua_pushstring(LuaState_to_lua_State(this), s);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

const char* LuaState::PushFString(const char *fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	const char* ret = lua_pushvfstring(LuaState_to_lua_State(this), fmt, argp);
	va_end(argp);
	return ret;
}

const char* LuaState::PushVFString(const char *fmt, va_list argp)
{
	return lua_pushvfstring(LuaState_to_lua_State(this), fmt, argp);
}

LuaStackObject LuaState::PushCClosure(lua_CFunction fn, int n)
{
	lua_pushcclosure(LuaState_to_lua_State(this), fn, n);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

LuaStackObject LuaState::PushCFunction(lua_CFunction f)
{
	lua_pushcclosure(LuaState_to_lua_State(this), f, 0);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

LuaStackObject LuaState::PushBoolean(int value)
{
	lua_pushboolean(LuaState_to_lua_State(this), value);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

LuaStackObject LuaState::PushLightUserdata(void* p)
{
	lua_pushlightuserdata(LuaState_to_lua_State(this), p);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

LuaStackObject LuaState::PushThread()
{
	lua_pushthread(LuaState_to_lua_State(this));
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}

LuaStackObject LuaState::Push(const LuaObject& obj)
{
	obj.Push(this);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}


// get functions (Lua -> stack)
void LuaState::GetTable(int index)
{
	lua_gettable(LuaState_to_lua_State(this), index);
}

void LuaState::GetField(int index, const char* key) {
	lua_getfield(LuaState_to_lua_State(this), index, key);
}


void LuaState::GetI(int index, lua_Integer key) {
#if LUA_VERSION_NUM >= 503
	lua_geti(LuaState_to_lua_State(this), index, key);
#else
	index = AbsIndex(index);
	lua_pushinteger(LuaState_to_lua_State(this), key);
	lua_gettable(LuaState_to_lua_State(this), index);
#endif
}


void LuaState::RawGet(int index)
{
	lua_rawget(LuaState_to_lua_State(this), index);
}

void LuaState::RawGetI(int index, lua_Integer n)
{
	lua_rawgeti(LuaState_to_lua_State(this), index, n);
}

void LuaState::RawGetP(int index, const void* p)
{
#if LUA_VERSION_NUM == 501
	assert(0);
#elif LUA_VERSION_NUM >= 502
	lua_rawgetp(LuaState_to_lua_State(this), index, p);
#endif
}

LuaStackObject LuaState::CreateTable(int narr, int nrec) {
	lua_createtable(LuaState_to_lua_State(this), narr, nrec);
	return LuaStackObject(*this, GetTop());
}


LuaStackObject LuaState::NewUserdata(size_t size) {
	lua_newuserdata(LuaState_to_lua_State(this), size);
	return LuaStackObject(*this, GetTop());
}


LuaStackObject LuaState::GetMetatable(int index)
{
	lua_getmetatable(LuaState_to_lua_State(this), index);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}


LuaStackObject LuaState::GetUservalue(int index) {
#if LUA_VERSION_NUM == 501
	assert(0);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
#elif LUA_VERSION_NUM >= 502
	lua_getuservalue(LuaState_to_lua_State(this), index);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
#endif
}

/*TODO
LuaStackObject LuaState::GetFEnv(int index) {
	lua_getfenv(LuaState_to_lua_State(this), index);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}
*/

LuaObject LuaState::GetGlobal(const char *name) {
	lua_State* L = LuaState_to_lua_State(this);
	lua_getglobal(L, name);
	return LuaObject(L, true);
}


LuaObject LuaState::GetGlobals() throw() {
#if LUA_VERSION_NUM == 501
	return LuaObject( this, LUA_GLOBALSINDEX );
#elif LUA_VERSION_NUM >= 502
	lua_pushglobaltable(LuaState_to_lua_State(this));
	return LuaObject(LuaState_to_lua_State(this), true);
#endif
}


LuaObject LuaState::GetRegistry() {
	return LuaObject(this, LUA_REGISTRYINDEX);  //{  lua_getregistry(LuaState_to_lua_State(this));
}


LuaStackObject LuaState::GetGlobals_Stack() {
#if LUA_VERSION_NUM == 501
	return LuaStackObject( this, LUA_GLOBALSINDEX );
#elif LUA_VERSION_NUM >= 502
	lua_pushglobaltable(LuaState_to_lua_State(this));
	return LuaStackObject(*this, GetTop());
#endif
}

LuaStackObject LuaState::GetGlobal_Stack(const char *name)
{
	lua_getglobal(LuaState_to_lua_State(this), name);  return LuaStackObject(*this, GetTop());
}

LuaStackObject LuaState::GetRegistry_Stack()
{
	return LuaStackObject(*this, LUA_REGISTRYINDEX);  //{  lua_getregistry(LuaState_to_lua_State(this));
}


// set functions(stack -> Lua)
void LuaState::SetGlobal(const char* key) {
	return lua_setglobal(LuaState_to_lua_State(this), key);
}

void LuaState::SetTable(int index)
{
	lua_settable(LuaState_to_lua_State(this), index);
}

void LuaState::SetField(int index, const char* key) {
	lua_setfield(LuaState_to_lua_State(this), index, key);
}


void LuaState::SetI(int index, lua_Integer key) {
#if LUA_VERSION_NUM >= 503
	lua_seti(LuaState_to_lua_State(this), index, key);
#else
	index = AbsIndex(index);
	lua_pushinteger(LuaState_to_lua_State(this), key);
	lua_insert(LuaState_to_lua_State(this), -2);
	lua_settable(LuaState_to_lua_State(this), index);
#endif
}


void LuaState::RawSet(int index)
{
	lua_rawset(LuaState_to_lua_State(this), index);
}

void LuaState::RawSetI(int index, lua_Integer n)
{
	lua_rawseti(LuaState_to_lua_State(this), index, n);
}

void LuaState::RawSetP(int index, const void* p)
{
#if LUA_VERSION_NUM == 501
	assert(0);
#elif LUA_VERSION_NUM >= 502
	lua_rawsetp(LuaState_to_lua_State(this), index, p);
#endif
}

void LuaState::SetMetatable(int index)
{
	lua_setmetatable(LuaState_to_lua_State(this), index);
}

void LuaState::SetUservalue(int index)
{
#if LUA_VERSION_NUM == 501
	assert(0);
#elif LUA_VERSION_NUM >= 502
	lua_setuservalue(LuaState_to_lua_State(this), index);
#endif
}

void LuaState::SetFEnv(int index)
{
#if LUA_VERSION_NUM <= 501
	lua_setfenv(LuaState_to_lua_State(this), index);
#else
	assert(0);
#endif
}

// `load' and `call' functions (load and run Lua code)
#if LUA_VERSION_NUM == 501  ||  LUA_VERSION_NUM == 502
void LuaState::CallK(int nargs, int nresults, int ctx, lua_CFunction k) {
	lua_call(LuaState_to_lua_State(this), nargs, nresults);
}
#elif LUA_VERSION_NUM == 502
void LuaState::CallK(int nargs, int nresults, int ctx, lua_CFunction k) {
	lua_callk(LuaState_to_lua_State(this), nargs, nresults, ctx, k);
}
#else
void LuaState::CallK(int nargs, int nresults, lua_KContext ctx, lua_KFunction k) {
	lua_callk(LuaState_to_lua_State(this), nargs, nresults, ctx, k);
}
#endif


void LuaState::Call(int nargs, int nresults) {
	lua_call(LuaState_to_lua_State(this), nargs, nresults);
}


#if LUA_VERSION_NUM == 501  ||  LUA_VERSION_NUM == 502
int LuaState::GetCtx(int* ctx) {
#if LUA_VERSION_NUM == 501
	return 0;
#elif LUA_VERSION_NUM >= 502
	return lua_getctx(LuaState_to_lua_State(this), ctx);
#endif
}
#endif


#if LUA_VERSION_NUM == 501
int LuaState::PCallK(int nargs, int nresults, int errfunc, int ctx, lua_CFunction k) {
	return lua_pcall(LuaState_to_lua_State(this), nargs, nresults, errfunc);
}
#elif LUA_VERSION_NUM == 502
int LuaState::PCallK(int nargs, int nresults, int errfunc, int ctx, lua_CFunction k) {
	return lua_pcallk(LuaState_to_lua_State(this), nargs, nresults, errfunc, ctx, k);
}
#else
int LuaState::PCallK(int nargs, int nresults, int errfunc, lua_KContext ctx, lua_KFunction k) {
	return lua_pcallk(LuaState_to_lua_State(this), nargs, nresults, errfunc, ctx, k);
}
#endif


int LuaState::PCall(int nargs, int nresults, int errf) {
	return lua_pcall(LuaState_to_lua_State(this), nargs, nresults, errf);
}


int LuaState::CPCall(lua_CFunction func, void* ud)
{
	lua_State* L = LuaState_to_lua_State(this);
	lua_pushcfunction(L, func);
	lua_pushlightuserdata(L, ud);
	return lua_pcall(L, 1, 0, 0);
}


int LuaState::Load(lua_Reader reader, void *data, const char *chunkname, const char *mode)
{
#if LUA_VERSION_NUM == 501
	return lua_load(LuaState_to_lua_State(this), reader, data, chunkname);
#elif LUA_VERSION_NUM >= 502
	return lua_load(LuaState_to_lua_State(this), reader, data, chunkname, mode);
#endif
}

#if LUA_ENDIAN_SUPPORT

int LuaState::Dump(lua_Writer writer, void* data, int strip, char endian)
{
//	return lua_dumpendian(LuaState_to_lua_State(this), writer, data, strip, endian);
#if LUA_VERSION_NUM <= 502
	return lua_dump(LuaState_to_lua_State(this), writer, data);
#else
	return lua_dump(LuaState_to_lua_State(this), writer, data, strip);
#endif
}

#else

int LuaState::Dump(lua_Writer writer, void* data, int strip)
{
#if LUA_VERSION_NUM <= 502
	return lua_dump(LuaState_to_lua_State(this), writer, data);
#else
	return lua_dump(LuaState_to_lua_State(this), writer, data, strip);
#endif
}

#endif /* LUA_ENDIAN_SUPPORT */

int LuaState::LoadFileX(const char* filename, const char* mode) {
#if LUA_VERSION_NUM == 501
	return luaL_loadfile(LuaState_to_lua_State(this), filename);
#elif LUA_VERSION_NUM >= 502
	return luaL_loadfilex(LuaState_to_lua_State(this), filename, mode);
#endif
}

int LuaState::LoadFile(const char* filename)
{
	return luaL_loadfile(LuaState_to_lua_State(this), filename);
}

int LuaState::DoFile(const char *filename)
{
	return luaL_dofile(LuaState_to_lua_State(this), filename);
}

int LuaState::DoString(const char *str)
{
	return luaL_dostring(LuaState_to_lua_State(this), str);
}

int LuaState::LoadBufferX(const char* buff, size_t size, const char* name, const char* mode) {
#if LUA_VERSION_NUM == 501
	return luaL_loadbuffer(LuaState_to_lua_State(this), buff, size, name);
#elif LUA_VERSION_NUM >= 502
	return luaL_loadbufferx(LuaState_to_lua_State(this), buff, size, name, mode);
#endif
}

int LuaState::LoadBuffer(const char* buff, size_t size, const char* name)
{
	return luaL_loadbuffer(LuaState_to_lua_State(this), buff, size, name);
}

int LuaState::DoBuffer(const char *buff, size_t size, const char *name)
{
	return (luaL_loadbuffer(LuaState_to_lua_State(this), buff, size, name) || lua_pcall(LuaState_to_lua_State(this), 0, 0, 0));
}

lua_Integer LuaState::L_len(int index)
{
#if LUA_VERSION_NUM == 501
	assert(0);
	return 0;
#elif LUA_VERSION_NUM >= 502
	return luaL_len(LuaState_to_lua_State(this), index);
#endif
}

void LuaState::SetFuncs(const luaL_Reg *l, int nup) {
#if LUA_VERSION_NUM == 501
	luaI_openlib(LuaState_to_lua_State(this), NULL, l, nup);
#elif LUA_VERSION_NUM >= 502
	luaL_setfuncs(LuaState_to_lua_State(this), l, nup);
#endif
}


int LuaState::GetSubTable(int idx, const char *fname) {
#if LUA_VERSION_NUM == 501
	assert(0);
	return 0;
#elif LUA_VERSION_NUM >= 502
	return luaL_getsubtable(LuaState_to_lua_State(this), idx, fname);
#endif
}


void LuaState::Traceback(lua_State *L1, const char *msg, int level) {
#if LUA_VERSION_NUM == 501
	assert(0);
#elif LUA_VERSION_NUM >= 502
	luaL_traceback(LuaState_to_lua_State(this), L1, msg, level);
#endif
}


void LuaState::RequireF(const char *modname, lua_CFunction openf, int glb) {
#if LUA_VERSION_NUM == 501
	assert(0);
#elif LUA_VERSION_NUM >= 502
	luaL_requiref(LuaState_to_lua_State(this), modname, openf, glb);
#endif
}


/*
** coroutine functions
*/
#if LUA_VERSION_NUM == 501
int LuaState::YieldK(int nresults, int ctx, lua_CFunction k) {
	return lua_yield(LuaState_to_lua_State(this), nresults);
}
#elif LUA_VERSION_NUM == 502
int LuaState::YieldK(int nresults, int ctx, lua_CFunction k) {
	return lua_yieldk(LuaState_to_lua_State(this), nresults, ctx, k);
}
#else
int LuaState::YieldK(int nresults, lua_KContext ctx, lua_KFunction k) {
	return lua_yieldk(LuaState_to_lua_State(this), nresults, ctx, k);
}
#endif


int LuaState::Yield_(int nresults) {
	return lua_yield(LuaState_to_lua_State(this), nresults);
}


int LuaState::Resume(lua_State *from, int nargs) {
#if LUA_VERSION_NUM == 501
	return lua_resume(LuaState_to_lua_State(this), nargs);
#elif LUA_VERSION_NUM >= 502
	return lua_resume(LuaState_to_lua_State(this), from, nargs);
#endif
}


int LuaState::Resume(LuaState *from, int nargs) {
#if LUA_VERSION_NUM == 501
	return lua_resume(LuaState_to_lua_State(this), nargs);
#elif LUA_VERSION_NUM >= 502
	return lua_resume(LuaState_to_lua_State(this), LuaState_to_lua_State(from), nargs);
#endif
}


int LuaState::Status() {
	return lua_status(LuaState_to_lua_State(this));
}


#if LUA_VERSION_NUM >= 503
int LuaState::IsYieldable() {
	return lua_isyieldable(LuaState_to_lua_State(this));
}
#endif


/*
** garbage-collection function and options
*/
int LuaState::GC(int what, int data)
{
	return lua_gc(LuaState_to_lua_State(this), what, data); 
}


/*
** miscellaneous functions
*/
int LuaState::Error()
{
	return lua_error(LuaState_to_lua_State(this));
}


int LuaState::Next(int index)
{
	return lua_next(LuaState_to_lua_State(this), index);
}


void LuaState::Concat(int n)
{
	lua_concat(LuaState_to_lua_State(this), n);
}


void LuaState::Len(int index)
{
#if LUA_VERSION_NUM == 501
	assert(0);
#elif LUA_VERSION_NUM >= 502
	lua_len(LuaState_to_lua_State(this), index);
#endif
}


size_t LuaState::StringToNumber(const char *s)
{
#if LUA_VERSION_NUM >= 503
    return lua_stringtonumber(LuaState_to_lua_State(this), s);
#else
    assert(0);
	return 0;
#endif
}

lua_Alloc LuaState::GetAllocF(void **ud)
{
	return lua_getallocf(LuaState_to_lua_State(this), ud);
}


void LuaState::SetAllocF(lua_Alloc f, void *ud)
{
	lua_setallocf(LuaState_to_lua_State(this), f, ud);
}

// Helper functions
void LuaState::Pop()
{
	lua_pop(LuaState_to_lua_State(this), 1);
}

void LuaState::Pop(int amount)
{
	lua_pop(LuaState_to_lua_State(this), amount);
}

void LuaState::NewTable() {
	lua_newtable(LuaState_to_lua_State(this));
}

void LuaState::Register(const char* key, lua_CFunction f) {
	lua_register(LuaState_to_lua_State(this), key, f);
}

size_t LuaState::StrLen(int index) {
#if LUA_VERSION_NUM == 501
	return lua_objlen(LuaState_to_lua_State(this), index);
#elif LUA_VERSION_NUM >= 502
	return lua_rawlen(LuaState_to_lua_State(this), index);
#endif
}

// Debug functions.
int LuaState::GetStack(int level, lua_Debug* ar)
{
	return lua_getstack(LuaState_to_lua_State(this), level, ar);
}

int LuaState::GetInfo(const char* what, lua_Debug* ar)
{
	return lua_getinfo(LuaState_to_lua_State(this), what, ar);
}

const char* LuaState::GetLocal(const lua_Debug* ar, int n)
{
	return lua_getlocal(LuaState_to_lua_State(this), ar, n);
}

const char* LuaState::SetLocal(const lua_Debug* ar, int n)
{
	return lua_setlocal(LuaState_to_lua_State(this), ar, n);
}

const char* LuaState::GetUpvalue(int funcindex, int n) {
	return lua_getupvalue(LuaState_to_lua_State(this), funcindex, n);
}

const char* LuaState::SetUpvalue(int funcindex, int n) {
	return lua_setupvalue(LuaState_to_lua_State(this), funcindex, n);
}

void* LuaState::UpvalueID(int fidx, int n) {
#if LUA_VERSION_NUM == 501
	assert(0);
	return NULL;
#elif LUA_VERSION_NUM >= 502
	return lua_upvalueid(LuaState_to_lua_State(this), fidx, n);
#endif
}

void LuaState::UpvalueJoin(int fidx1, int n1, int fidx2, int n2) {
#if LUA_VERSION_NUM == 501
	return;
#elif LUA_VERSION_NUM >= 502
	lua_upvaluejoin(LuaState_to_lua_State(this), fidx1, n1, fidx2, n2);
#endif
}

void LuaState::SetHook(lua_Hook func, int mask, int count)
{
	lua_sethook(LuaState_to_lua_State(this), func, mask, count);
}

lua_Hook LuaState::GetHook()
{
	return lua_gethook(LuaState_to_lua_State(this));
}

int LuaState::GetHookMask()
{
	return lua_gethookmask(LuaState_to_lua_State(this));
}

int LuaState::GetHookCount()
{
	return lua_gethookcount(LuaState_to_lua_State(this));
}


// fastref support
#if LUA_FASTREF_SUPPORT

int LuaState::FastRef() {
	return lua_fastref(LuaState_to_lua_State(this));
}


int LuaState::FastRefIndex(int index) {
	return lua_fastrefindex(LuaState_to_lua_State(this), index);
}


void LuaState::FastUnref(int ref) {
	return lua_fastunref(LuaState_to_lua_State(this), ref);
}


void LuaState::GetFastRef(int ref) {
	return lua_getfastref(LuaState_to_lua_State(this), ref);
}

#endif /* LUA_FASTREF_SUPPORT */


// lauxlib functions.
void LuaState::OpenLib(const char *libname, const luaL_Reg *l, int nup) {
#if LUA_VERSION_NUM == 501
	luaI_openlib(LuaState_to_lua_State(this), libname, l, nup);
#elif LUA_VERSION_NUM >= 502
	assert(0);
#endif
}


void LuaState::NewLib(const luaL_Reg *l, int nup) {
#if LUA_VERSION_NUM == 501
	assert(0);
#elif LUA_VERSION_NUM >= 502
	lua_State* L = LuaState_to_lua_State(this);
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, l, nup);
#endif
}


void LuaState::LRegister(const char *libname, const luaL_Reg *l) {
#if LUA_VERSION_NUM == 501
	luaL_register(LuaState_to_lua_State(this), libname, l);
#elif LUA_VERSION_NUM >= 502
	lua_State* L = LuaState_to_lua_State(this);
	lua_createtable(L, 0, 0);
	luaL_setfuncs(L, l, 0);
	lua_setglobal(L, libname);
#endif
}


int LuaState::GetMetaField(int obj, const char *e) {
	return luaL_getmetafield(LuaState_to_lua_State(this), obj, e);
}


int LuaState::CallMeta(int obj, const char *e) {
	return luaL_callmeta(LuaState_to_lua_State(this), obj, e);
}


int LuaState::TypeError(int narg, const char* tname)
{
	lua_State* L = LuaState_to_lua_State(this);
	const char *msg = lua_pushfstring(L, "%s expected, got %s",
		tname, luaL_typename(L, narg));
	return luaL_argerror(L, narg, msg);
}


int LuaState::ArgError(int narg, const char* extramsg)
{
	return luaL_argerror(LuaState_to_lua_State(this), narg, extramsg);
}


const char* LuaState::CheckLString(int numArg, size_t* len)
{
	return luaL_checklstring(LuaState_to_lua_State(this), numArg, len);
}


const char* LuaState::OptLString(int numArg, const char *def, size_t* len)
{
	return luaL_optlstring(LuaState_to_lua_State(this), numArg, def, len);
}


lua_Number LuaState::CheckNumber(int numArg)
{
	return luaL_checknumber(LuaState_to_lua_State(this), numArg);
}


lua_Number LuaState::OptNumber(int nArg, lua_Number def)
{
	return luaL_optnumber(LuaState_to_lua_State(this), nArg, def);
}


lua_Integer LuaState::CheckInteger(int numArg)
{
	return luaL_checkinteger(LuaState_to_lua_State(this), numArg);
}


lua_Integer LuaState::OptInteger(int nArg, lua_Integer def)
{
	return luaL_optinteger(LuaState_to_lua_State(this), nArg, def);
}


#if LUA_VERSION_NUM == 501

unsigned int LuaState::CheckUnsigned(int numArg)
{
	return (unsigned int)luaL_checkinteger(LuaState_to_lua_State(this), numArg);
}


unsigned int LuaState::OptUnsigned(int nArg, unsigned int def)
{
	return (unsigned int)luaL_optinteger(LuaState_to_lua_State(this), nArg, def);
}

#elif LUA_VERSION_NUM == 502

lua_Unsigned LuaState::CheckUnsigned(int numArg)
{
	return luaL_checkunsigned(LuaState_to_lua_State(this), numArg);
}


lua_Unsigned LuaState::OptUnsigned(int nArg, lua_Unsigned def)
{
	return luaL_optunsigned(LuaState_to_lua_State(this), nArg, def);
}

#else

lua_Unsigned LuaState::CheckUnsigned(int numArg)
{
	return (lua_Unsigned)luaL_checkinteger(LuaState_to_lua_State(this), numArg);
}


lua_Unsigned LuaState::OptUnsigned(int nArg, lua_Unsigned def)
{
	return (lua_Unsigned)luaL_optinteger(LuaState_to_lua_State(this), nArg, (lua_Integer)def);
}

#endif


lua_Integer luaL_checkboolean (lua_State *L, int narg) {
	lua_Integer d = lua_toboolean(L, narg);
	if (d == 0 && !lua_isboolean(L, narg)) {  /* avoid extra test when d is not 0 */
		const char *msg = lua_pushfstring(L, "%s expected, got %s",
				luaL_typename(L, narg), lua_typename(L, LUA_TBOOLEAN));
		return luaL_argerror(L, narg, msg);
	}
	return d;
}


lua_Integer LuaState::CheckBoolean(int numArg) {
	return luaL_checkboolean(LuaState_to_lua_State(this), numArg);
}


lua_Integer LuaState::OptBoolean(int nArg, lua_Integer def) {
	return luaL_opt(LuaState_to_lua_State(this), luaL_checkboolean, nArg, def);
}


const char* LuaState::CheckString(int numArg)
{
	return luaL_checkstring(LuaState_to_lua_State(this), numArg);
}


const char* LuaState::OptString(int numArg, const char* def)
{
	return luaL_optlstring(LuaState_to_lua_State(this), numArg, def, NULL);
}


int LuaState::CheckInt(int numArg)
{
#if LUA_VERSION_NUM >= 503
	return (int)luaL_checkinteger(LuaState_to_lua_State(this), numArg);
#else
	return (int)luaL_checkint(LuaState_to_lua_State(this), numArg);
#endif
}


long LuaState::CheckLong(int numArg)
{
#if LUA_VERSION_NUM >= 503
	return (long)luaL_checkinteger(LuaState_to_lua_State(this), numArg);
#else
	return (long)luaL_checklong(LuaState_to_lua_State(this), numArg);
#endif
}


int LuaState::OptInt(int numArg, int def)
{
#if LUA_VERSION_NUM >= 503
	return (int)luaL_optinteger(LuaState_to_lua_State(this), numArg, def);
#else
	return (int)luaL_optint(LuaState_to_lua_State(this), numArg, def);
#endif
}


long LuaState::OptLong(int numArg, int def)
{
#if LUA_VERSION_NUM >= 503
	return (long)luaL_optinteger(LuaState_to_lua_State(this), numArg, def);
#else
	return (long)luaL_optlong(LuaState_to_lua_State(this), numArg, def);
#endif
}


void LuaState::CheckStack(int sz, const char* msg)
{
	luaL_checkstack(LuaState_to_lua_State(this), sz, msg);
}


void LuaState::CheckType(int narg, int t)
{
	luaL_checktype(LuaState_to_lua_State(this), narg, t);
}


void LuaState::CheckAny(int narg)
{
	luaL_checkany(LuaState_to_lua_State(this), narg);
}


LuaObject LuaState::NewMetatable(const char* tname)
{
	luaL_newmetatable(LuaState_to_lua_State(this), tname);
	return LuaObject(LuaState_to_lua_State(this), true);
}


void* LuaState::TestUData(int ud, const char* tname) {
#if LUA_VERSION_NUM == 501
	assert(0);
	return 0;
#elif LUA_VERSION_NUM >= 502
	return luaL_testudata(LuaState_to_lua_State(this), ud, tname);
#endif
}


void* LuaState::CheckUData(int ud, const char* tname) {
	return luaL_checkudata(LuaState_to_lua_State(this), ud, tname);
}


void LuaState::Where(int lvl) {
	luaL_where(LuaState_to_lua_State(this), lvl);
}


int LuaState::Error(const char* fmt, ...) {
	va_list argp;
	va_start(argp, fmt);
	luaL_where(LuaState_to_lua_State(this), 1);
	lua_pushvfstring(LuaState_to_lua_State(this), fmt, argp);
	va_end(argp);
	lua_concat(LuaState_to_lua_State(this), 2);
	return lua_error(LuaState_to_lua_State(this));
}


int LuaState::CheckOption(int narg, const char *def, const char *const lst[]) {
	return luaL_checkoption(LuaState_to_lua_State(this), narg, def, lst);
}


int LuaState::FileResult(int stat, const char *fname) {
#if LUA_VERSION_NUM == 501
	assert(0);
	return 0;
#elif LUA_VERSION_NUM >= 502
	return luaL_fileresult(LuaState_to_lua_State(this), stat, fname);
#endif
}


int LuaState::ExecResult(int stat) {
#if LUA_VERSION_NUM == 501
	assert(0);
	return 0;
#elif LUA_VERSION_NUM >= 502
	return luaL_execresult(LuaState_to_lua_State(this), stat);
#endif
}


int LuaState::Ref(int t)
{
	return luaL_ref(LuaState_to_lua_State(this), t);
}


void LuaState::Unref(int t, int ref)
{
	luaL_unref(LuaState_to_lua_State(this), t, ref);
}


void LuaState::ArgCheck(bool condition, int numarg, const char* extramsg)
{
	luaL_argcheck(LuaState_to_lua_State(this), condition, numarg, extramsg);
}


const char* LuaState::GSub(const char *s, const char *p, const char *r)
{
	return luaL_gsub(LuaState_to_lua_State(this), s, p, r);
}



LuaStackObject LuaState::PushCClosure(int (*f)(LuaState*), int n)
{
	unsigned char* buffer = (unsigned char*)lua_newuserdata(LuaState_to_lua_State(this), sizeof(f));
	memcpy(buffer, &f, sizeof(f));
	Insert(-n-1);
	lua_pushcclosure(LuaState_to_lua_State(this), LPCD::LuaStateFunctionDispatcher, n + 1);
	return LuaStackObject(this, lua_gettop(LuaState_to_lua_State(this)));
}


const char* LuaState::LTypeName(int index) {
	return luaL_typename(LuaState_to_lua_State(this), index);
}


LuaObject LuaState::GetMetatable(const char* metatableName) {
	luaL_getmetatable(LuaState_to_lua_State(this), metatableName);
	return LuaObject(LuaState_to_lua_State(this), true);
}


int LuaState::UpvalueIndex(int i)
{
	return lua_upvalueindex(i);
}

int LuaState::LoadString(const char* str)
{
	return luaL_loadbuffer(LuaState_to_lua_State(this), str, strlen(str), str);
}

// Extra
LuaStackObject LuaState::BoxPointer(void* u)
{
	(*(void **)(lua_newuserdata(LuaState_to_lua_State(this), sizeof(void *))) = (u));  return LuaStackObject(*this, lua_gettop(LuaState_to_lua_State(this)));
}

void* LuaState::UnBoxPointer(int stackIndex)
{
	return (*(void **)(lua_touserdata(LuaState_to_lua_State(this), stackIndex)));
}


LuaObject LuaState::NewUserdataBox(void* u) {
	LuaObject obj(this);
	obj.Assign(this, u);
	return obj;
}


int LuaState::DoString( const char *str, LuaObject& fenvObj ) {
	lua_State* L = LuaState_to_lua_State(this);
	int status = luaL_loadbuffer(L, str, strlen(str), str);
	if (status != 0)
		return status;
	fenvObj.Push(L);
	SetFEnv(-2);
	return lua_pcall(L, 0, LUA_MULTRET, 0);
}


int LuaState::DoFile( const char *filename, LuaObject& fenvObj ) {
	lua_State* L = LuaState_to_lua_State(this);
	int status = luaL_loadfile(L, filename);
	if (status != 0)
		return status;
	fenvObj.Push(L);
	SetFEnv(-2);
	return lua_pcall(L, 0, LUA_MULTRET, 0);
}


int LuaState::DoBuffer( const char *buff, size_t size, const char *name, LuaObject& fenvObj ) {
	lua_State* L = LuaState_to_lua_State(this);
	int status = luaL_loadbuffer(L, buff, size, name);
	if (status != 0)
		return status;
	fenvObj.Push(L);
	SetFEnv(-2);
	return lua_pcall(L, 0, LUA_MULTRET, 0);
}


LuaObject LuaState::GetLocalByName( int level, const char* name )
{
	lua_State * L = GetCState();
	lua_Debug ar;
	int i;
	const char *localName;
	if (lua_getstack(L, level, &ar) == 0)
		return LuaObject(this);  /* failure: no such level in the stack */
	i = 1;
	while ((localName = lua_getlocal(L, &ar, i++)) != NULL) {
		if (strcmp(name, localName) == 0)
		{
			LuaObject obj(this, -1);
			lua_pop(L, 1);
			return obj;
		}
		lua_pop(L, 1);  /* remove variable value */
	}
	return LuaObject(this);
}

} // namespace LuaPlus

extern "C" {
#include "lualib.h"
}

namespace LuaPlus {

int pmain (lua_State *L)
{
	luaL_openlibs(L);
	return 0;
}


/**
**/
void LuaState::OpenLibs()
{
//#if LUAPLUS_INCLUDE_STANDARD_LIBRARY
	lua_State* L = LuaState_to_lua_State(this);
	int top = lua_gettop(L);
	lua_pushcfunction(L, &pmain);
	lua_pcall(L, 0, 0, 0);
	lua_settop(L, top);
//#endif // LUAPLUS_INCLUDE_STANDARD_LIBRARY
}

} // namespace LuaPlus
