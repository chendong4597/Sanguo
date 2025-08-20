/*!
 * \file LuaStateCD.cpp
 *
 * \author zjhlogo
 * \date 2019/09/25
 *
 * 
 */
#include "LuaStateCD.h"

namespace LPCD
{
    int LuaStateFunctionDispatcher(lua_State* L) {
        typedef int(*Functor)(LuaPlus::LuaState*);
        unsigned char* buffer = LPCD::GetFirstUpvalueAsUserdata(L);
        Functor& func = *(Functor*)(buffer);
        return (*func)(lua_State_to_LuaState(L));
    }

}
