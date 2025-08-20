/*!
 * \file LuaPlusCD.cpp
 *
 * \author zjhlogo
 * \date 2019/09/25
 *
 * 
 */
#include <cassert>
#include "LuaPlusCD.h"

namespace LPCD {

    void Push(lua_State* L, const char* value, int len)
    {
        lua_pushlstring(L, value, len);
    }

    unsigned char* GetFirstUpvalueAsUserdata(lua_State* L)
    {
        void* buffer;

#ifndef FAST_DISPATCH
        buffer = lua_touserdata(L, lua_upvalueindex(1));
#else // FAST_DISPATCH
#endif // FAST_DISPATCH

        return (unsigned char*)buffer;
    }

    int lua_StateFunctionDispatcher(lua_State* L)
    {
        typedef int(*Functor)(lua_State*);
        unsigned char* buffer = GetFirstUpvalueAsUserdata(L);
        Functor& func = *(Functor*)(buffer);
        return (*func)(L);
    }

    void lpcd_pushfunctorclosure(lua_State* L, int(*func)(lua_State*), unsigned int nupvalues = 0)
    {
        unsigned char* buffer = (unsigned char*)lua_newuserdata(L, sizeof(func));
        memcpy(buffer, &func, sizeof(func));
        lua_insert(L, -1 - (int)nupvalues);
        lua_pushcclosure(L, LPCD::lua_StateFunctionDispatcher, nupvalues + 1);
    }

    void* GetObjectUserdata(lua_State* L)
    {
        int type = lua_type(L, 1);
        if (type == LUA_TUSERDATA)
            return *(void **)(lua_touserdata(L, 1));
        else if (type == LUA_TTABLE) {
            lua_pushstring(L, "__object");
            lua_rawget(L, 1);

            void* ret;
            int subType = lua_type(L, -1);
            if (subType == LUA_TLIGHTUSERDATA) {
                ret = lua_touserdata(L, -1);
            }
            else if (subType == LUA_TUSERDATA) {
                ret = *(void **)(lua_touserdata(L, -1));
            }
            else {
                ret = NULL;
                luaL_error(L, "The table does not have a userdata member called __object.");
            }
            lua_pop(L, 1);
            return ret;
        }
        else {
            luaL_argerror(L, 1, "must be userdata or a table with a userdata member called __object");
        }

        return NULL;
    }

    void* GetInPlaceObjectUserdata(lua_State* L)
    {
        int type = lua_type(L, 1);
        if (type == LUA_TUSERDATA)
            return lua_touserdata(L, 1);
        else if (type == LUA_TTABLE) {
            lua_pushstring(L, "__object");
            lua_rawget(L, 1);

            void* ret;
            int subType = lua_type(L, -1);
            if (subType == LUA_TLIGHTUSERDATA) {
                ret = lua_touserdata(L, -1);
            }
            else if (subType == LUA_TUSERDATA) {
                ret = lua_touserdata(L, -1);
            }
            else {
                ret = NULL;
                luaL_error(L, "The table does not have a userdata member called __object.");
            }
            lua_pop(L, 1);
            return ret;
        }
        else {
            luaL_argerror(L, 1, "must be userdata or a table with a userdata member called __object");
        }

        return NULL;
    }

    int PropertyMetatable_newindex(lua_State* L) {
        // table key value
        lua_pushvalue(L, 2);						// table key value key
        lua_rawget(L, lua_upvalueindex(1));			// table key value property
        if (lua_isfunction(L, -1)) {
            if (lua_getupvalue(L, -1, 2)) {			// table key value property userdataTest
                if (lua_touserdata(L, -1) == (void*)-2) {
                    lua_pop(L, 1);								// table key value property
                    lua_CFunction f = lua_tocfunction(L, -1);	// table key value propertyfunc
                    lua_getupvalue(L, -1, 1);					// table key value propertyfunc offset
                    lua_replace(L, 4);							// table key value offset
                    return f(L);
                }
                lua_pop(L, 1);						// table key value
            }
            lua_rawset(L, -3);						// table
            return 0;
        }
        else if (!lua_isnil(L, -1)) {
            lua_pop(L, 1);							// table key value
            lua_rawset(L, -3);						// table
            return 0;
        }

        lua_pop(L, 1);								// table key value

        if (!lua_getmetatable(L, -3))				// table key value metatable
            return 0;

        do {
            lua_getfield(L, -1, "__newindex");				// table key value metatable __newindex
            if (lua_isnil(L, -1)) {
                lua_pop(L, 2);								// table key value
                lua_rawset(L, -3);							// table
                return 0;
            }

            lua_getupvalue(L, -1, 1);						// table key value metatable __newindex functions
            lua_pushvalue(L, 2);							// table key value metatable __newindex functions key
            lua_rawget(L, -2);								// table key value metatable __newindex functions property
            if (lua_isfunction(L, -1)) {
                if (lua_getupvalue(L, -1, 2)) {				// table key value metatable __newindex functions userdataTest
                    if (lua_touserdata(L, -1) == (void*)-2) {
                        lua_pop(L, 1);						// table key value metatable __newindex functions
                        lua_CFunction f = lua_tocfunction(L, -1);	// table key value metatable __newindex functions
                        lua_getupvalue(L, -1, 1);			// table key value metatable __newindex functions offset
                        lua_replace(L, 4);					// table key value offset __newindex functions
                        return f(L);
                    }
                    lua_pop(L, 2);							// table key value metatable __newindex
                }
                lua_rawset(L, -3);							// table key value
                return 1;
            }
            else if (!lua_isnil(L, -1)) {
                lua_pop(L, 4);
                lua_rawset(L, -3);
                return 0;
            }

            lua_pop(L, 3);									// table key value metatable
            if (!lua_getmetatable(L, 4)) {					// table key value metatable metatable2
                lua_pop(L, 1);
                break;
            }
            lua_remove(L, -2);								// table key value metatable2
        } while (true);

        lua_rawset(L, -3);
        return 0;
    }

    // function gettable_event (table, key)
    // upvalues:
    //     1 - function__index
    //     2 - properties__index
    int PropertyMetatable_index(lua_State* L) {
        lua_pushvalue(L, 2);							// table key key
        lua_rawget(L, lua_upvalueindex(1));				// table key property
        if (lua_isfunction(L, -1)) {
            if (lua_getupvalue(L, -1, 2)) {
                if (lua_touserdata(L, -1) == (void*)-2) {
                    lua_pop(L, 1);
                    lua_CFunction f = lua_tocfunction(L, -1);	// table key propertyfunc
                    lua_getupvalue(L, -1, 1);					// table key propertyfunc offset
                    lua_replace(L, 3);							// table key offset
                    return f(L);
                }
                lua_pop(L, 1);
            }
            return 1;
        }
        else if (!lua_isnil(L, -1))
            return 1;

        lua_pop(L, 1);									// table key

        if (!lua_getmetatable(L, -2))					// table key metatable
            return 0;

        do {
            lua_getfield(L, -1, "__index");					// table key metatable __index
            if (lua_isnil(L, -1))
                return 0;

            lua_getupvalue(L, -1, 1);						// table key metatable __index functions
            lua_pushvalue(L, 2);							// table key metatable __index functions key
            lua_rawget(L, -2);								// table key metatable __index functions property
            if (lua_isfunction(L, -1)) {
                if (lua_getupvalue(L, -1, 2)) {				// table key metatable __index functions userdataTest
                    if (lua_touserdata(L, -1) == (void*)-2) {
                        lua_pop(L, 1);						// table key metatable __index functions
                        lua_CFunction f = lua_tocfunction(L, -1);	// table key metatable __index functions
                        lua_getupvalue(L, -1, 1);			// table key metatable __index functions offset
                        lua_replace(L, 3);					// table key offset __index functions
                        return f(L);
                    }
                    lua_pop(L, 1);							// table key metatable __index functions
                }
                return 1;
            }
            else if (!lua_isnil(L, -1))
                return 1;

            lua_pop(L, 3);									// table key metatable
            if (!lua_getmetatable(L, 3))					// table key metatable metatable2
                return 0;
            lua_remove(L, -2);								// table key metatable2
        } while (true);
    }
}


void lpcd_integratepropertysupport(lua_State* L, int metatableIndex, bool inPlace) {
    metatableIndex = lpcd_abs_index(L, metatableIndex);
    lua_pushstring(L, "__index");		// metatable "__index"
    lua_newtable(L);					// metatable "__index" __index
    lua_pushboolean(L, inPlace ? 1 : 0);// metatable "__index" __index inPlace
    lua_pushcclosure(L, LPCD::PropertyMetatable_index, 2);	// metatable "__index" PropertyMetatable_index
    lua_rawset(L, metatableIndex);		// metatable

    lua_pushstring(L, "__newindex");	// metatable "__newindex"
    lua_newtable(L);					// metatable "__newindex" __newindex
    lua_pushboolean(L, inPlace ? 1 : 0);// metatable "__newindex" __newindex inPlace
    lua_pushcclosure(L, LPCD::PropertyMetatable_newindex, 2);	// metatable "__newindex" PropertyMetatable_newindex
    lua_rawset(L, metatableIndex);		// metatable
}


void lpcd_propertymetatable_getfunctions(lua_State* L, int metatableIndex) {
    lua_getfield(L, metatableIndex, "__index");		// (metatable) __index
    lua_getupvalue(L, -1, 1);						// (metatable) __index props__functions
    lua_remove(L, -2);								// (metatable) props__functions
}


void lpcd_newclassmetatable(lua_State* L, const char* className, const char* baseClassName, bool inPlace) {
    luaL_newmetatable(L, className);			// class_metatable
    lpcd_integratepropertysupport(L, -1, inPlace);

    if (baseClassName) {
        luaL_getmetatable(L, baseClassName);	// class_metatable baseClass_metatable
        lua_setmetatable(L, -2);				// class_metatable
    }
}


void* lpcd_checkobject(lua_State* L, int index, const char* tname, bool throwError) {
    assert(tname);

    int type = lua_type(L, index);
    if (type == LUA_TTABLE) {
        lua_getfield(L, index, "__object");
        if (lua_getmetatable(L, -1)) {  /* does it have a metatable? */
            lua_getfield(L, LUA_REGISTRYINDEX, tname);  /* get correct metatable */
            int equal = lua_rawequal(L, -1, -2);	/* does it have the correct mt? */
            lua_pop(L, 2);  /* remove both metatables */
            if (!equal) {
                lua_pop(L, 1);
                goto error;
            }
        }
        void *p = lua_touserdata(L, -1);
        lua_pop(L, 1);
        return p;
    }
    else if (type == LUA_TUSERDATA) {
        if (lua_getmetatable(L, index)) {  /* does it have a metatable? */
            lua_getfield(L, LUA_REGISTRYINDEX, tname);  /* get correct metatable */
            int equal = lua_rawequal(L, -1, -2);	/* does it have the correct mt? */
            lua_pop(L, 2);  /* remove both metatables */
            if (!equal)
                goto error;
        }
        return lua_touserdata(L, index);
    }
error:
    if (throwError) {
        const char *msg = lua_pushfstring(L, "%s expected, got %s", tname, luaL_typename(L, index));
        luaL_argerror(L, index, msg);
    }
    return NULL;
}
