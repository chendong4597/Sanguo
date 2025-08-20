/*!
 * \file TextInputUtil_Android.cpp
 *
 * \author laizhisheng
 * \date 2017/01/18 15:31
 *
 */

#include "TextInputUtil_Android.h"
#include "JniHelper.h"
#include "alloca.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <mutex>
#include <jni.h>
#include "../../LogUtil.h"

NS_BEGIN

TextInputUtil& TextInputUtil::getInstance()
{
    static TextInputUtil_Android s_TextInputUtil_Android;
    return s_TextInputUtil_Android;
}

void TextInputUtil_Android::showInputBox(const tstring& strTitle, const tstring& strContent, INPUT_BOX_MODE eMode, INPUT_BOX_FLAG eFlag, int maxLength, INPUT_BOX_CALLBACK callback, void* param)
{
	// TODO:
}

void TextInputUtil_Android::showIMEKeyboard(bool show)
{
	// TODO:
    if (show == true)
    {
        JniMethodInfo methodInfo;
		if (!JniHelper::getStaticMethodInfo(methodInfo, "com/igg/bzbee/static_app/OpenGLESView", "openIMEKeyboard", "()V")) { return; }

		LOGI("CallStaticVoidMethod openIMEKeyboard");

        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
    }
    else
    {
        JniMethodInfo MethodInfo;
		if (!JniHelper::getStaticMethodInfo(MethodInfo, "com/igg/bzbee/static_app/OpenGLESView", "closeIMEKeyboard", "()V")) { return; }

		LOGI("CallStaticVoidMethod closeIMEKeyboard");

        MethodInfo.env->CallStaticVoidMethod(MethodInfo.classID, MethodInfo.methodID);
    }
}

void TextInputUtil_Android::updateNativeFrame(int left, int top, int maxWidth, int maxHeight)
{
	JniMethodInfo methodInfo;
	if (!JniHelper::getStaticMethodInfo(methodInfo, "com/igg/bzbee/static_app/OpenGLESView", "setEditBoxViewRect", "(IIII)V")) { return; }

	LOGI("CallStaticVoidMethod updateNativeFrame");

	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, left, top, maxWidth, maxHeight);
}

NS_END

