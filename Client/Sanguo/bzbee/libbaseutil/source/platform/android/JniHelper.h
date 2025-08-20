#pragma once

#include <jni.h>
#include <string>
#include <vector>
#include <unordered_map>

class JniMethodInfo
{
public:
    ~JniMethodInfo();

public:
    JNIEnv* env{};
    jclass classID{};
    jmethodID methodID{};
};

class JniHelper
{
	JniHelper();
	~JniHelper();
public:
    static JavaVM* getJavaVM();
    static void setJavaVM(JavaVM* javaVM);

    static bool getEnv(JNIEnv** env);

    static bool getStaticMethodInfo(JniMethodInfo& methodinfo, const char* className, const char* methodName, const char* paramCode);
    static bool getMethodInfo(JniMethodInfo& methodinfo, const char* className, const char* methodName, const char* paramCode);
    static std::string callStaticStringMethod(const std::string& className, const std::string& methodName);

private:
    static jclass getClassID(const char* className, JNIEnv* env = nullptr);

private:
    static JavaVM* m_javaVm;

};
