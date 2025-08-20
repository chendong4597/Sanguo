/*!
* \file ScriptVM.cpp
*
* \author zhichenxia
* \date 2017/12/04
*
*
*/
#include "ScriptVM.h"
#include "LuaPlus/LuaPlus.h"
#include "../file/File.h"
#include "../file/FileSystem.h"
#include "../FileUtil.h"
#include "../LogUtil.h"
#include <list>

using namespace NS;
using namespace LuaPlus;


std::vector<ScriptVM::FuncLuaBind> ScriptVM::s_funcBindList;

void ScriptVM::registerFunction(FuncLuaBind func)
{
    s_funcBindList.push_back(func);
}

ScriptVM::~ScriptVM(void)
{
	stop();
}

ScriptVM& ScriptVM::getInstance(void)
{
    static ScriptVM* s_instance = new ScriptVM();
	return *s_instance;
}

void ScriptVM::reboot(void)
{
	stop();

	m_pState = LuaState::Create(true);
    auto luaObj = m_pState->GetGlobals();

    for (const auto& v : s_funcBindList)
    {
        v(luaObj);
    }
    
	execString("package.path = './script/?.lua';");

	execFile("system/boot.lua");
}


void ScriptVM::stop(bool bPrint)
{
	if (!m_pState) return;

	LuaState::Destroy(m_pState);
	m_pState = nullptr;

	if (bPrint) printf("\n---------------- Lua VM stopped. ----------------\n\n\n\n");
}


void ScriptVM::execFile(const tstring& filepath)
{
	if (!m_pState) return;
	tstring path = "script/" + filepath;

#ifdef WIN32
	if (filepath.size() >= 2 && filepath[1] == ':') path = filepath;
#endif

	if (m_pState->DoFile(path.c_str()))
	{
		printf("%s\n\n", m_pState->ToString(-1));
		LOGE(m_pState->ToString(-1));
	}
}


void ScriptVM::execString(const tstring& str)
{
	if (!m_pState) return;

	if (m_pState->DoString(str.c_str()))
	{
		printf("%s\n\n", m_pState->ToString(-1));
		LOGE(m_pState->ToString(-1));
	}
}

extern "C" int readable_res(char const* str)
{
    NS::tstring assetPath{ str };
    if (assetPath.length() > 2 && assetPath[0] == '.' && assetPath[1] == '/') assetPath = assetPath.substr(2);
    return NS::FileSystem::isFileExist(assetPath);
}

extern "C" char* fread_res(char const* str, unsigned int* pLength)
{
    NS::tstring assetPath{ str };
    if (assetPath.length() > 2 && assetPath[0] == '.' && assetPath[1] == '/') assetPath = assetPath.substr(2);

    auto file = NS::FileSystem::openFile(assetPath);
    if (!file) return nullptr;

    unsigned int fileSize = static_cast<unsigned int>(file->getFileSize());
    auto p = malloc(fileSize);
    file->read(p, fileSize);
    *pLength = fileSize;

    return reinterpret_cast<char*>(p);
}
