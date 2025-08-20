/*!
 * \file TextInputUtil_Android.h
 *
 * \author laizhisheng
 * \date 2017/01/18 15:30
 *
 */

#pragma once

#include "../../TextInputUtil.h"

NS_BEGIN

class TextInputUtil_Android : public TextInputUtil
{
public:
	RTTI_DEF(TextInputUtil_Android);

    virtual void showInputBox(const tstring& strTitle, const tstring& strContent, INPUT_BOX_MODE eMode, INPUT_BOX_FLAG eFlag, int maxLength, INPUT_BOX_CALLBACK callback, void* param) override;
    virtual void showIMEKeyboard(bool show) override;
	void updateNativeFrame(int left, int top, int maxWidth, int maxHeight) override;
};

NS_END