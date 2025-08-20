/*!
* \file ScriptVM.h
*
* \author zhichenxia
* \date 2017/12/04
*
*
*/
#pragma once
#include "../BaseType.h"

namespace LuaPlus
{
    class LuaState;
    class LuaObject;
}

class ScriptVM final
{
public:
    typedef bool(*FuncLuaBind)(LuaPlus::LuaObject& luaObj);

public:
	static ScriptVM& getInstance(void);
	~ScriptVM(void);
	static inline LuaPlus::LuaState* getState(void) { return getInstance().m_pState; }

	void reboot(void);
	void stop(bool bPrint = true);

	void execFile(const NS::tstring& filepath);
	void execString(const NS::tstring& str);

public:
    static void registerFunction(FuncLuaBind func);

private:
	LuaPlus::LuaState* m_pState{};
    static std::vector<FuncLuaBind> s_funcBindList;

};
