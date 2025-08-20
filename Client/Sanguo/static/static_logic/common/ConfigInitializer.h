/*!
 * \file SandboxConfigInitializer.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 05/10/2016
 *
 * 
 */
#pragma once 
#include <vector>


class ConfigInitializer
{
public:
	static bool initialize();
	static bool initializeAfterRevisionUpdate();
	static bool terminate();
	static bool loadLanguage();
	static bool ReloadConfig();
	static bool LoadConfig();
	static bool UnloadConfig();
};
