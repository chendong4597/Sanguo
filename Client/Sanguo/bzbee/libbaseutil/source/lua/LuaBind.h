/*!
 * \file LuaBind.h
 *
 * \author zjhlogo
 * \date 2019/10/29
 *
 * 
 */
#pragma once
#include "../BaseType.h"
#include "LuaPlus/LuaPlus.h"
#include "LuaPlus/LuaStackObject.h"
#include "LuaPlus/LuaObject.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>

NS_BEGIN
class Rtti;
NS_END

class LuaBind
{
public:
    typedef std::unordered_set<NS::tstring> LuaMethodSet;
    typedef void(*FuncClassBinding)(LuaPlus::LuaObject&, LuaMethodSet&);

    class LuaMetaTableInfo
    {
    public:
        NS::tstring className;
        LuaMethodSet methodSet;
        LuaPlus::LuaObject metaTable;
    };

public:
    static void addClassBindingFunc(const NS::tstring& className, FuncClassBinding func);
    static const LuaPlus::LuaObject& getClassMetaTable(const NS::Rtti* rtti);
    static std::shared_ptr<LuaPlus::LuaObject> createLuaObject(const NS::Rtti* rtti, void* p_this, lua_State* L);
    static void pushClass(const NS::Rtti* rtti, void* p_this, lua_State* L);
    static void getClassObject(LuaPlus::LuaObject& objOut, const NS::Rtti* rtti, void* p_this);

private:
    static void initializeMetaTable(LuaMetaTableInfo& info, const NS::Rtti* rtti);

public:
    static std::unordered_map<NS::tstring, FuncClassBinding> s_classBindingMap;
    static std::unordered_map<NS::tstring, LuaMetaTableInfo> s_luaMetaTableMap;

};
