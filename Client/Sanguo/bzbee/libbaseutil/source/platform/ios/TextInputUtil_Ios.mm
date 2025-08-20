/*!
 * \file TextInputUtil_Ios.cpp
 *
 * \date 2018/09/10
 * 
 * 
 * \author lzj
 */

#include "TextInputUtil_Ios.h"
#include <EAGLView.h>

NS_BEGIN

TextInputUtil& TextInputUtil::getInstance()
{
    static TextInputUtil_Ios s_TextInputUtil_Ios;
    return s_TextInputUtil_Ios;
}

void TextInputUtil_Ios::showInputBox(const tstring& strTitle, const tstring& strContent, INPUT_BOX_MODE eMode, INPUT_BOX_FLAG eFlag, int maxLength, INPUT_BOX_CALLBACK callback, void* param)
{
}

void TextInputUtil_Ios::showIMEKeyboard(bool show)
{
    [[EAGLView sharedEGLView] showIMEKeyboard:show];
}

NS_END
