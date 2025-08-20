/*!
 * \file LuaCallback.cpp
 *
 * \author zjhlogo
 * \date 2019/10/30
 *
 * 
 */
#include "LuaCallback.h"
#include "ScriptVM.h"
#include "LuaPlus/LuaState.h"

namespace LPCD
{
    void Type<glm::vec2>::Push(lua_State* L, glm::vec2 value)
    {
        lua_createtable(L, 0, 2);
        auto table = lua_gettop(L);

        lua_getglobal(L, "__meta_vec2");
        lua_setmetatable(L, table);

        lua_pushnumber(L, (lua_Number)value.x);
        lua_setfield(L, table, "x");

        lua_pushnumber(L, (lua_Number)value.y);
        lua_setfield(L, table, "y");
    }

    glm::vec2 Type<glm::vec2>::Get(lua_State* L, int idx)
    {
        lua_getfield(L, idx, "x");
        float x = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        lua_getfield(L, idx, "y");
        float y = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        return glm::vec2(x, y);
    }

    bool Type<glm::vec2>::Match(lua_State* L, int idx)
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

        return true;
    }


    void Type<glm::vec3>::Push(lua_State* L, glm::vec3 value)
    {
        lua_createtable(L, 0, 3);
        auto table = lua_gettop(L);

        lua_getglobal(L, "__meta_vec3");
        lua_setmetatable(L, table);

        lua_pushnumber(L, (lua_Number)value.x);
        lua_setfield(L, table, "x");

        lua_pushnumber(L, (lua_Number)value.y);
        lua_setfield(L, table, "y");

        lua_pushnumber(L, (lua_Number)value.z);
        lua_setfield(L, table, "z");
    }

    glm::vec3 Type<glm::vec3>::Get(lua_State* L, int idx)
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

    bool Type<glm::vec3>::Match(lua_State* L, int idx)
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


    void Type<glm::vec4>::Push(lua_State* L, glm::vec4 value)
    {
        lua_createtable(L, 0, 4);
        auto table = lua_gettop(L);

        lua_getglobal(L, "__meta_vec4");
        lua_setmetatable(L, table);

        lua_pushnumber(L, (lua_Number)value.x);
        lua_setfield(L, table, "x");

        lua_pushnumber(L, (lua_Number)value.y);
        lua_setfield(L, table, "y");

        lua_pushnumber(L, (lua_Number)value.z);
        lua_setfield(L, table, "z");

        lua_pushnumber(L, (lua_Number)value.w);
        lua_setfield(L, table, "w");
    }

    glm::vec4 Type<glm::vec4>::Get(lua_State* L, int idx)
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

        lua_getfield(L, idx, "w");
        float w = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        return glm::vec4(x, y, z, w);
    }

    bool Type<glm::vec4>::Match(lua_State* L, int idx)
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

        lua_getfield(L, idx, "w");
        b = lua_isnumber(L, -1);
        lua_pop(L, 1);
        if (!b) return false;

        return true;
    }


    void Type<glm::quat>::Push(lua_State* L, glm::quat value)
    {
        lua_createtable(L, 0, 4);
        auto table = lua_gettop(L);

        lua_getglobal(L, "__meta_quat");
        lua_setmetatable(L, table);

        lua_pushnumber(L, (lua_Number)value.x);
        lua_setfield(L, table, "x");

        lua_pushnumber(L, (lua_Number)value.y);
        lua_setfield(L, table, "y");

        lua_pushnumber(L, (lua_Number)value.z);
        lua_setfield(L, table, "z");

        lua_pushnumber(L, (lua_Number)value.w);
        lua_setfield(L, table, "w");
    }

    glm::quat Type<glm::quat>::Get(lua_State* L, int idx)
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

        lua_getfield(L, idx, "w");
        float w = static_cast<float>(lua_tonumber(L, -1));
        lua_pop(L, 1);

        return glm::quat(w, x, y, z);
    }

    bool Type<glm::quat>::Match(lua_State* L, int idx)
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

        lua_getfield(L, idx, "w");
        b = lua_isnumber(L, -1);
        lua_pop(L, 1);
        if (!b) return false;

        return true;
    }


    void Type<glm::mat4>::Push(lua_State* L, glm::mat4 value)
    {
        lua_createtable(L, 0, 16);
        auto table = lua_gettop(L);

        lua_getglobal(L, "__meta_mat4");
        lua_setmetatable(L, table);

        lua_pushnumber(L, (lua_Number)value[0].x); lua_setfield(L, table, "x1");
        lua_pushnumber(L, (lua_Number)value[0].y); lua_setfield(L, table, "y1");
        lua_pushnumber(L, (lua_Number)value[0].z); lua_setfield(L, table, "z1");
        lua_pushnumber(L, (lua_Number)value[0].w); lua_setfield(L, table, "w1");

        lua_pushnumber(L, (lua_Number)value[1].x); lua_setfield(L, table, "x2");
        lua_pushnumber(L, (lua_Number)value[1].y); lua_setfield(L, table, "y2");
        lua_pushnumber(L, (lua_Number)value[1].z); lua_setfield(L, table, "z2");
        lua_pushnumber(L, (lua_Number)value[1].w); lua_setfield(L, table, "w2");

        lua_pushnumber(L, (lua_Number)value[2].x); lua_setfield(L, table, "x3");
        lua_pushnumber(L, (lua_Number)value[2].y); lua_setfield(L, table, "y3");
        lua_pushnumber(L, (lua_Number)value[2].z); lua_setfield(L, table, "z3");
        lua_pushnumber(L, (lua_Number)value[2].w); lua_setfield(L, table, "w3");

        lua_pushnumber(L, (lua_Number)value[3].x); lua_setfield(L, table, "x4");
        lua_pushnumber(L, (lua_Number)value[3].y); lua_setfield(L, table, "y4");
        lua_pushnumber(L, (lua_Number)value[3].z); lua_setfield(L, table, "z4");
        lua_pushnumber(L, (lua_Number)value[3].w); lua_setfield(L, table, "w4");
    }

    glm::mat4 Type<glm::mat4>::Get(lua_State* L, int idx)
    {
        lua_getfield(L, idx, "x1"); float x1 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "y1"); float y1 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "z1"); float z1 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "w1"); float w1 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);

        lua_getfield(L, idx, "x2"); float x2 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "y2"); float y2 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "z2"); float z2 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "w2"); float w2 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);

        lua_getfield(L, idx, "x3"); float x3 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "y3"); float y3 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "z3"); float z3 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "w3"); float w3 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);

        lua_getfield(L, idx, "x4"); float x4 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "y4"); float y4 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "z4"); float z4 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);
        lua_getfield(L, idx, "w4"); float w4 = static_cast<float>(lua_tonumber(L, -1)); lua_pop(L, 1);

        return glm::mat4
        (
            x1, y1, z1, w1,
            x2, y2, z2, w2,
            x3, y3, z3, w3,
            x4, y4, z4, w4
        );
    }

    bool Type<glm::mat4>::Match(lua_State* L, int idx)
    {
        int b = lua_istable(L, idx);
        if (!b) return false;

        lua_getfield(L, idx, "x1"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "y1"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "z1"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "w1"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;

        lua_getfield(L, idx, "x2"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "y2"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "z2"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "w2"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;

        lua_getfield(L, idx, "x3"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "y3"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "z3"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "w3"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;

        lua_getfield(L, idx, "x4"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "y4"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "z4"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;
        lua_getfield(L, idx, "w4"); b = lua_isnumber(L, -1); lua_pop(L, 1); if (!b) return false;

        return true;
    }
}

LuaTable::LuaTable()
{
    data().AssignNewTable(ScriptVM::getState()->GetCState());
}
