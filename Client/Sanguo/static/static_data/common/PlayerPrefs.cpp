#include "stdafx.h"
#include "PlayerPrefs.h"

#ifndef GENERATE_GET_FUNCTION
#define GENERATE_GET_FUNCTION(TYPE, NAME, DEFAULT_RET)\
TYPE PlayerPrefs::Get##NAME(const std::string& key)\
{\
    auto it = m_##NAME##HashMap.find(key);\
    if (it == m_##NAME##HashMap.end())\
    {\
        return DEFAULT_RET;\
    }\
    return it->second;\
}
#endif

#ifndef GENERATE_SET_FUNCTION
#define GENERATE_SET_FUNCTION(TYPE, NAME)\
void PlayerPrefs::Set##NAME(const std::string& key, const TYPE& value)\
{\
    auto it = m_##NAME##HashMap.find(key);\
    if (it == m_##NAME##HashMap.end() && HasKey(key))\
    {\
        return;\
    }\
    m_##NAME##HashMap[key] = value;\
}
#endif

IntHashMap PlayerPrefs::m_IntHashMap;
FloatHashMap PlayerPrefs::m_FloatHashMap;
StringHashMap PlayerPrefs::m_StringHashMap;
BoolHashMap PlayerPrefs::m_BoolHashMap;

void PlayerPrefs::DeleteAll()	//Removes all keys and values from the preferences.Use with caution.
{
    m_IntHashMap.clear();
    m_FloatHashMap.clear();
    m_StringHashMap.clear();
    m_BoolHashMap.clear();
}

void PlayerPrefs::DeleteKey(const std::string& key)	//Removes key and its corresponding value from the preferences.
{
    //TODO
}

GENERATE_GET_FUNCTION(float, Float, 0.0f)

GENERATE_GET_FUNCTION(int, Int, 0)

GENERATE_GET_FUNCTION(std::string, String, "")

bool PlayerPrefs::HasKey(const std::string& key)	//Returns true if key exists in the preferences.
{
    auto it_int = m_IntHashMap.find(key);
    auto it_float = m_FloatHashMap.find(key);
    auto it_string = m_StringHashMap.find(key);
    auto it_bool = m_BoolHashMap.find(key);

    return !(it_int == m_IntHashMap.end() && it_float == m_FloatHashMap.end() 
        && it_string == m_StringHashMap.end() && it_bool == m_BoolHashMap.end());
}

void PlayerPrefs::Save()	//Writes all modified preferences to disk.
{
    //TODO
}

GENERATE_SET_FUNCTION(float, Float)

GENERATE_SET_FUNCTION(int, Int)

GENERATE_SET_FUNCTION(std::string, String)

GENERATE_SET_FUNCTION(bool, Bool)

GENERATE_GET_FUNCTION(bool, Bool, false)
