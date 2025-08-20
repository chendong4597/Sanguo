/*!
 * \file LuaHelper.cpp
 *
 * \author zjhlogo
 * \date 2019/09/25
 *
 * 
 */
#include "LuaStackObject.h"
#include "LuaTableIterator.h"
#include "LuaObject.h"
#include "LuaHelper.h"

namespace LuaPlus
{
    namespace LuaHelper
    {


        bool GetBoolean(const LuaObject& obj, int key, bool require, bool defaultValue) {
            LuaObject boolObj = obj[key];
            if (!boolObj.IsBoolean()) {
                if (require) {
                    luaplus_assert(0);
                }
                return defaultValue;
            }
            return boolObj.GetBoolean();
        }


        bool GetBoolean(const LuaObject& obj, const char* key, bool require, bool defaultValue) {
            LuaObject boolObj = obj[key];
            if (!boolObj.IsBoolean()) {
                if (require) {
                    luaplus_assert(0);
                }
                return defaultValue;
            }
            return boolObj.GetBoolean();
        }


        lua_Integer GetInteger(const LuaObject& obj, int key, bool require, int defaultValue) {
            LuaObject intObj = obj[key];
            if (!intObj.IsInteger()) {
                if (require) {
                    luaplus_assert(0);
                }
                return defaultValue;
            }
            return intObj.GetInteger();
        }


        lua_Integer GetInteger(const LuaObject& obj, const char* key, bool require, int defaultValue) {
            LuaObject intObj = obj[key];
            if (!intObj.IsInteger()) {
                if (require) {
                    luaplus_assert(0);
                }
                return defaultValue;
            }
            return intObj.GetInteger();
        }


        float GetFloat(const LuaObject& obj, int key, bool require, float defaultValue) {
            LuaObject floatObj = obj[key];
            if (!floatObj.IsNumber()) {
                if (require) {
                    luaplus_assert(0);
                }
                return defaultValue;
            }
            return (float)floatObj.GetNumber();
        }


        float GetFloat(const LuaObject& obj, const char* key, bool require, float defaultValue) {
            LuaObject floatObj = obj[key];
            if (!floatObj.IsNumber()) {
                if (require) {
                    luaplus_assert(0);
                }
                return defaultValue;
            }
            return (float)floatObj.GetNumber();
        }


        void* GetLightUserdata(const LuaObject& obj, int key, bool require, void* defaultValue) {
            LuaObject outObj = obj[key];
            if (!outObj.IsLightUserdata()) {
                if (require) {
                    luaplus_assert(0);
                }
                return defaultValue;
            }
            return outObj.GetLightUserdata();
        }


        void* GetLightUserdata(const LuaObject& obj, const char* key, bool require, void* defaultValue) {
            LuaObject outObj = obj[key];
            if (!outObj.IsLightUserdata()) {
                if (require) {
                    luaplus_assert(0);
                }
                return defaultValue;
            }
            return outObj.GetLightUserdata();
        }


        const char* GetString(const LuaObject& obj, int key, bool require, const char* defaultValue) {
            LuaObject stringObj = obj[key];
            if (!stringObj.IsString()) {
                if (require) {
                    luaplus_assert(0);
                }
                return defaultValue;
            }
            return stringObj.GetString();
        }


        const char* GetString(const LuaObject& obj, const char* key, bool require, const char* defaultValue) {
            LuaObject stringObj = obj[key];
            if (!stringObj.IsString()) {
                if (require) {
                    luaplus_assert(0);
                }
                return defaultValue;
            }
            return stringObj.GetString();
        }


        LuaObject GetTable(const LuaObject& obj, int key, bool require) {
            LuaObject tableObj = obj[key];
            if (!tableObj.IsTable()) {
                if (require) {
                    luaplus_assert(0);
                }
            }
            return tableObj;
        }


        LuaObject GetTable(const LuaObject& obj, const char* key, bool require) {
            LuaObject tableObj = obj[key];
            if (!tableObj.IsTable()) {
                if (require) {
                    luaplus_assert(0);
                }
            }
            return tableObj;
        }


        void MergeObjects(LuaObject& mergeTo, const LuaObject& mergeFrom, bool replaceDuplicates)
        {
            if (mergeTo.GetState() == mergeFrom.GetState())
            {
                for (LuaTableIterator it(mergeFrom); it; ++it)
                {
                    LuaObject toNodeKeyObj = mergeTo[it.GetKey()];
                    if (it.GetValue().IsTable())
                    {
                        if (toNodeKeyObj.IsNil() || replaceDuplicates)
                        {
                            toNodeKeyObj = mergeTo.CreateTable(it.GetKey());
                        }
                        MergeObjects(toNodeKeyObj, it.GetValue(), replaceDuplicates);
                    }
                    else if (toNodeKeyObj.IsNil() || replaceDuplicates)
                    {
                        mergeTo.Set(it.GetKey(), it.GetValue());
                    }
                }
            }
            else
            {
                for (LuaTableIterator it(mergeFrom); it; ++it)
                {
                    LuaObject obj;
                    switch (it.GetKey().Type())
                    {
                    case LUA_TBOOLEAN:	obj.Assign(mergeTo.GetState(), it.GetKey().GetBoolean());		break;
                    case LUA_TNUMBER:	obj.Assign(mergeTo.GetState(), it.GetKey().GetNumber());			break;
                    case LUA_TSTRING:	obj.Assign(mergeTo.GetState(), it.GetKey().GetString());			break;
                    }

                    LuaObject toNodeKeyObj = mergeTo[obj];

                    if (it.GetValue().IsTable())
                    {
                        if (toNodeKeyObj.IsNil() || replaceDuplicates)
                        {
                            toNodeKeyObj = mergeTo.CreateTable(obj);
                        }
                        MergeObjects(toNodeKeyObj, it.GetValue(), replaceDuplicates);
                    }
                    else if (toNodeKeyObj.IsNil() || replaceDuplicates)
                    {
                        LuaObject toKeyObj;
                        switch (it.GetKey().Type())
                        {
                        case LUA_TBOOLEAN:	toKeyObj.Assign(mergeTo.GetState(), it.GetKey().GetBoolean());		break;
                        case LUA_TNUMBER:	toKeyObj.Assign(mergeTo.GetState(), it.GetKey().GetNumber());			break;
                        case LUA_TSTRING:	toKeyObj.Assign(mergeTo.GetState(), it.GetKey().GetString());			break;
                        }

                        switch (it.GetValue().Type())
                        {
                        case LUA_TBOOLEAN:	mergeTo.Set(toKeyObj, it.GetValue().GetBoolean());	break;
                        case LUA_TNUMBER:	mergeTo.Set(toKeyObj, it.GetValue().GetNumber());		break;
                        case LUA_TSTRING:	mergeTo.Set(toKeyObj, it.GetValue().GetString());		break;
                        }
                    }
                }
            }
        }
    }
}
