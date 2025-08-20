/*!
 * \file TextInputUtil_Ios.h
 *
 * \date 2018/09/10
 * 
 * 
 * \author lzj
 */

#pragma once

#include "../../TextInputUtil.h"

NS_BEGIN

class TextInputUtil_Ios : public TextInputUtil
{
public:
    RTTI_DEF(TextInputUtil_Ios);

    void showInputBox(const tstring& strTitle, const tstring& strContent, INPUT_BOX_MODE eMode, INPUT_BOX_FLAG eFlag, int maxLength, INPUT_BOX_CALLBACK callback, void* param) override;
    void showIMEKeyboard(bool show) override;

};

NS_END
