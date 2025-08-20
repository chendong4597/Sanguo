#pragma once

#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, int> IntHashMap;
typedef std::unordered_map<std::string, float> FloatHashMap;
typedef std::unordered_map<std::string, std::string> StringHashMap;
typedef std::unordered_map<std::string, bool> BoolHashMap;

class PlayerPrefs
{
public:
    static void DeleteAll();	//Removes all keys and values from the preferences.Use with caution.
    static void DeleteKey(const std::string& key);	//Removes key and its corresponding value from the preferences.
    static float GetFloat(const std::string& key);	//Returns the value corresponding to key in the preference file if it exists.
    static int GetInt(const std::string& key);	//Returns the value corresponding to key in the preference file if it exists.
    static std::string GetString(const std::string& key);	//Returns the value corresponding to key in the preference file if it exists.
    static bool HasKey(const std::string& key);	//Returns true if key exists in the preferences.
    static void Save();	//Writes all modified preferences to disk.
    static void SetFloat(const std::string& key, const float& value);	//Sets the value of the preference identified by key.
    static void SetInt(const std::string& key, const int& value);	//Sets the value of the preference identified by key.
    static void SetString(const std::string& key, const std::string& value);	//Sets the value of the preference identified by key.

    static void SetBool(const std::string& key, const bool& value);
    static bool GetBool(const std::string& key);

private:
    static IntHashMap m_IntHashMap;
    static FloatHashMap m_FloatHashMap;
    static StringHashMap m_StringHashMap;
    static BoolHashMap m_BoolHashMap;
};