/*!
 * \file TextInputUtil_Win32.cpp
 *
 * \author laizhisheng
 * \date 2017/01/19 17:11
 *
 */

#include "TextInputUtil_Win32.h"
#include <mutex>
#include <thread>
#include <windows.h>

#ifdef USE_SDL
#include <SDL/SDL_keyboard.h>
#endif

NS_BEGIN

TextInputUtil& TextInputUtil::getInstance()
{
	static TextInputUtil_Win32 s_TextInputUtil_Win32;
	return s_TextInputUtil_Win32;
}

void TextInputUtil_Win32::showInputBox(const tstring& strTitle, const tstring& strContent, INPUT_BOX_MODE eMode, INPUT_BOX_FLAG eFlag, int maxLength, INPUT_BOX_CALLBACK callback, void* param)
{
	// TODO: nothing to do
}

void TextInputUtil_Win32::showIMEKeyboard(bool show)
{
	// TODO: 
#ifdef USE_SDL
    if (show == true)
        SDL_StartTextInput();
    else
        SDL_StopTextInput();
#endif
}

NS_END