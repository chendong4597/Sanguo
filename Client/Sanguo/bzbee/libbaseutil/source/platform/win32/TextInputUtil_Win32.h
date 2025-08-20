/*!
 * \file TextInputUtil_Win32.h
 *
 * \author laizhisheng
 * \date 2017/01/19 17:10
 *
 */

#pragma once
#include "../../TextInputUtil.h"

NS_BEGIN

class TextInputUtil_Win32 : public TextInputUtil
{
public:
	RTTI_DEF(TextInputUtil_Win32);

	virtual void showInputBox(const tstring& strTitle, const tstring& strContent, INPUT_BOX_MODE eMode, INPUT_BOX_FLAG eFlag, int maxLength, INPUT_BOX_CALLBACK callback, void* param) override;
	virtual void showIMEKeyboard(bool show) override;

};

NS_END