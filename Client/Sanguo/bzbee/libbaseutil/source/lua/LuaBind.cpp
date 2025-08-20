/*!
 * \file LuaBind.cpp
 *
 * \author zjhlogo
 * \date 2019/10/29
 *
 * 
 */
#include "LuaBind.h"
#include "LuaPlus/LuaStackObject.h"
#include "LuaPlus/LuaObject.h"
#include "ScriptVM.h"
#include "../Rtti.h"

std::unordered_map<NS::tstring, LuaBind::FuncClassBinding> LuaBind::s_classBindingMap;
std::unordered_map<NS::tstring, LuaBind::LuaMetaTableInfo> LuaBind::s_luaMetaTableMap;

void LuaBind::addClassBindingFunc(const NS::tstring& className, FuncClassBinding func)
{
    s_classBindingMap.emplace(className, func);
}

const LuaPlus::LuaObject& LuaBind::getClassMetaTable(const NS::Rtti* rtti)
{
    LuaBind::LuaMetaTableInfo* info = nullptr;

    auto it = s_luaMetaTableMap.find(rtti->getClassName());
    if (it == s_luaMetaTableMap.end())
    {
        info = &s_luaMetaTableMap[rtti->getClassName()];
        initializeMetaTable(*info, rtti);
    }
    else
    {
        info = &it->second;
    }

    return info->metaTable;
}

std::shared_ptr<LuaPlus::LuaObject> LuaBind::createLuaObject(const NS::Rtti* rtti, void* p_this, lua_State* L)
{
    assert(rtti);

    const auto& metaTable = getClassMetaTable(rtti);

    LuaPlus::LuaObject* p = new LuaPlus::LuaObject();
    p->AssignNewTable(ScriptVM::getState());
    p->SetMetatable(metaTable);
    p->SetLightUserdata("__object", const_cast<void*>(p_this));

    return std::shared_ptr<LuaPlus::LuaObject>(p, [](LuaPlus::LuaObject* p)
    {
        p->SetNil("__object");
        delete p;
    });
}

void LuaBind::pushClass(const NS::Rtti* rtti, void* p_this, lua_State* L)
{
    assert(rtti);

    const auto& metaTable = getClassMetaTable(rtti);

    LuaPlus::LuaObject p;
    p.AssignNewTable(ScriptVM::getState());
    p.SetMetatable(metaTable);
    p.SetLightUserdata("__object", p_this);
    p.Push(L);
}

void LuaBind::getClassObject(LuaPlus::LuaObject& objOut, const NS::Rtti* rtti, void* p_this)
{
    assert(rtti);

    const auto& metaTable = getClassMetaTable(rtti);

    objOut.AssignNewTable(ScriptVM::getState());
    objOut.SetMetatable(metaTable);
    objOut.SetLightUserdata("__object", p_this);
}

void LuaBind::initializeMetaTable(LuaMetaTableInfo& info, const NS::Rtti* rtti)
{
    info.className = rtti->getClassName();
    info.metaTable.AssignNewTable(ScriptVM::getState());
    info.metaTable.SetObject("__index", ScriptVM::getState()->GetGlobal("__meta_index"));

    const NS::Rtti* currRtti = rtti;
    do
    {
        auto it = s_classBindingMap.find(currRtti->getClassName());
        if (it != s_classBindingMap.end())
        {
            it->second(info.metaTable, info.methodSet);
        }
        currRtti = currRtti->getBaseRtti();
    } while (currRtti);
}
