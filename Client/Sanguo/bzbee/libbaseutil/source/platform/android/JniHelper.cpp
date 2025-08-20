#include "JniHelper.h"
#include "../../LogUtil.h"
#include <string.h>

extern "C"
{
    static bool getEnv(JNIEnv** env)
    {
        bool bRet = false;

        do
        {
            if (JniHelper::getJavaVM()->GetEnv((void**)env, JNI_VERSION_1_4) != JNI_OK)
            {
                NS::LogUtil::logException("Failed to get the environment using GetEnv()");
                NS::LogUtil::reportException("Jni Exception");
                break;
            }

            if (JniHelper::getJavaVM()->AttachCurrentThread(env, 0) < 0)
            {
                NS::LogUtil::logException("Failed to get the environment using AttachCurrentThread()");
                NS::LogUtil::reportException("Jni Exception");
                break;
            }

            bRet = true;
        } while (0);

        return bRet;
    }

    static jclass getClassID_(const char* className, JNIEnv* env)
    {
        JNIEnv *pEnv = env;
        jclass ret = 0;

        do
        {
            if (!pEnv)
            {
                if (!getEnv(&pEnv))
                {
                    break;
                }
            }

            ret = pEnv->FindClass(className);
            if (!ret)
            {
                NS::LogUtil::logException(fmt::format("Failed to find class of {0}", className));
                NS::LogUtil::reportException("Jni Exception");
                break;
            }
        } while (0);

        return ret;
    }

    static bool getStaticMethodInfo_(JniMethodInfo& methodinfo, const char* className, const char* methodName, const char* paramCode)
    {
        jmethodID methodID = nullptr;
        JNIEnv* pEnv = nullptr;
        bool bRet = false;

        do
        {
            if (!getEnv(&pEnv))
            {
                break;
            }

            jclass classID = getClassID_(className, pEnv);

            methodID = pEnv->GetStaticMethodID(classID, methodName, paramCode);
            if (!methodID)
            {
                pEnv->DeleteLocalRef(classID);

                NS::LogUtil::logException(fmt::format("Failed to find static method id of {0}", methodName));
                NS::LogUtil::reportException("Jni Exception");
                break;
            }

            methodinfo.classID = classID;
            methodinfo.env = pEnv;
            methodinfo.methodID = methodID;

            bRet = true;
        } while (0);

        return bRet;
    }

    static bool getMethodInfo_(JniMethodInfo& methodinfo, const char* className, const char* methodName, const char* paramCode)
    {
        jmethodID methodID = nullptr;
        JNIEnv *pEnv = nullptr;
        bool bRet = false;

        do
        {
            if (!getEnv(&pEnv))
            {
                break;
            }

            jclass classID = getClassID_(className, pEnv);

            methodID = pEnv->GetMethodID(classID, methodName, paramCode);
            if (!methodID)
            {
                pEnv->DeleteLocalRef(classID);

                NS::LogUtil::logException(fmt::format("Failed to find method id of {0}", methodName));
                NS::LogUtil::reportException("Jni Exception");
                break;
            }

            methodinfo.classID = classID;
            methodinfo.env = pEnv;
            methodinfo.methodID = methodID;

            bRet = true;
        } while (0);

        return bRet;
    }
}

static std::string jstring2string_(jstring jstr)
{
    if (jstr == nullptr) return "";

    JNIEnv *env = 0;
    if (!getEnv(&env)) return 0;

    const char* chars = env->GetStringUTFChars(jstr, nullptr);
    std::string ret(chars);
    env->ReleaseStringUTFChars(jstr, chars);

    return ret;
}

JniMethodInfo::~JniMethodInfo()
{
    if (env)
    {
        if (classID)
        {
            env->DeleteLocalRef(classID);
            classID = nullptr;
        }
    }
}

JniHelper::JniHelper()
{
}

JniHelper::~JniHelper()
{
}

JavaVM* JniHelper::m_javaVm = nullptr;

JavaVM* JniHelper::getJavaVM()
{
    return m_javaVm;
}

void JniHelper::setJavaVM(JavaVM* javaVm)
{
    m_javaVm = javaVm;
}

bool JniHelper::getEnv(JNIEnv** env)
{
    return ::getEnv(env);
}

jclass JniHelper::getClassID(const char* className, JNIEnv* env)
{
    return getClassID_(className, env);
}

bool JniHelper::getStaticMethodInfo(JniMethodInfo& methodinfo, const char* className, const char* methodName, const char* paramCode)
{
    return getStaticMethodInfo_(methodinfo, className, methodName, paramCode);
}

bool JniHelper::getMethodInfo(JniMethodInfo& methodinfo, const char* className, const char* methodName, const char* paramCode)
{
    return getMethodInfo_(methodinfo, className, methodName, paramCode);
}

std::string JniHelper::callStaticStringMethod(const std::string& className, const std::string& methodName)
{
    std::string ret;

    JniMethodInfo t;
    if (getStaticMethodInfo(t, className.c_str(), methodName.c_str(), "()Ljava/lang/String;"))
    {
        jstring jret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        ret = jstring2string_(jret);
        t.env->DeleteLocalRef(jret);
    }

    return ret;
}
