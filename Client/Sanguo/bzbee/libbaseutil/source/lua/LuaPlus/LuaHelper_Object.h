///////////////////////////////////////////////////////////////////////////////
// This source file is part of the LuaPlus source distribution and is Copyright
// 2001-2011 by Joshua C. Jensen (jjensen@workspacewhiz.com).
//
// The latest version may be obtained from http://luaplus.org/.
//
// The code presented in this file may be used in any environment it is
// acceptable to use Lua.
///////////////////////////////////////////////////////////////////////////////
#ifndef LUAPLUS__LUAHELPER_OBJECT_H
#define LUAPLUS__LUAHELPER_OBJECT_H

#include "LuaPlusInternal.h"

///////////////////////////////////////////////////////////////////////////////
// namespace LuaPlus
///////////////////////////////////////////////////////////////////////////////
namespace LuaPlus
{

/**
**/
namespace LuaHelper
{
	/**
		Attempts retrieval of the value from the passed in LuaObject.

		\param obj The LuaObject representing a table to attempt retrieval
			of the value from.
		\param key The lookup key for the value.  The key may be of number,
			string, or LuaObject type.
		\param require If set to true, the key must exist in the table and
			must be of the requested type.  If it doesn't exist or isn't of
			the type, an assertion fires.  Otherwise, defaultValue is 
			returned.
		\param defaultValue The default value to be returned if require is
			not true and the key doesn't exist or the found value is not
			of the right type.
		\return Returns the value found or the defaultValue.
	**/
	bool GetBoolean( const LuaObject& obj, int key, bool require = true, bool defaultValue = false );
	bool GetBoolean( const LuaObject& obj, const char* key, bool require = true, bool defaultValue = false );


	/**
		Attempts retrieval of the value from the passed in LuaObject.

		\param obj The LuaObject representing a table to attempt retrieval
			of the value from.
		\param key The lookup key for the value.  The key may be of number,
			string, or LuaObject type.
		\param require If set to true, the key must exist in the table and
			must be of the requested type.  If it doesn't exist or isn't of
			the type, an assertion fires.  Otherwise, defaultValue is 
			returned.
		\param defaultValue The default value to be returned if require is
			not true and the key doesn't exist or the found value is not
			of the right type.
		\return Returns the value found or the defaultValue.
	**/
	lua_Integer GetInteger( const LuaObject& obj, int key, bool require = true, int defaultValue = -1 );
	lua_Integer GetInteger( const LuaObject& obj, const char* key, bool require = true, int defaultValue = -1 );


	/**
		Attempts retrieval of the value from the passed in LuaObject.

		\param obj The LuaObject representing a table to attempt retrieval
			of the value from.
		\param key The lookup key for the value.  The key may be of number,
			string, or LuaObject type.
		\param require If set to true, the key must exist in the table and
			must be of the requested type.  If it doesn't exist or isn't of
			the type, an assertion fires.  Otherwise, defaultValue is 
			returned.
		\param defaultValue The default value to be returned if require is
			not true and the key doesn't exist or the found value is not
			of the right type.
		\return Returns the value found or the defaultValue.
	**/
	float GetFloat( const LuaObject& obj, int key, bool require = true, float defaultValue = -1.0f );
	float GetFloat( const LuaObject& obj, const char* key, bool require = true, float defaultValue = -1.0f );


	/**
		Attempts retrieval of the value from the passed in LuaObject.

		\param obj The LuaObject representing a table to attempt retrieval
			of the value from.
		\param key The lookup key for the value.  The key may be of number,
			string, or LuaObject type.
		\param require If set to true, the key must exist in the table and
			must be of the requested type.  If it doesn't exist or isn't of
			the type, an assertion fires.  Otherwise, defaultValue is 
			returned.
		\param defaultValue The default value to be returned if require is
			not true and the key doesn't exist or the found value is not
			of the right type.
		\return Returns the value found or the defaultValue.
	**/
	void* GetLightUserdata( const LuaObject& obj, int key, bool require = true, void* defaultValue = NULL );
	void* GetLightUserdata( const LuaObject& obj, const char* key, bool require = true, void* defaultValue = NULL );


	/**
		Attempts retrieval of the value from the passed in LuaObject.

		\param obj The LuaObject representing a table to attempt retrieval
			of the value from.
		\param key The lookup key for the value.  The key may be of number,
			string, or LuaObject type.
		\param require If set to true, the key must exist in the table and
			must be of the requested type.  If it doesn't exist or isn't of
			the type, an assertion fires.  Otherwise, defaultValue is 
			returned.
		\param defaultValue The default value to be returned if require is
			not true and the key doesn't exist or the found value is not
			of the right type.
		\return Returns the value found or the defaultValue.
	**/
	const char* GetString( const LuaObject& obj, int key, bool require = true, const char* defaultValue = "" );
	const char* GetString( const LuaObject& obj, const char* key, bool require = true, const char* defaultValue = "" );


	/**
		Attempts retrieval of the obj from the passed in LuaObject.

		\param obj The LuaObject representing a table to attempt retrieval
			of the value from.
		\param key The lookup key for the value.  The key may be of number,
			string, or LuaObject type.
		\param require If set to true, the key must exist in the table and
			must be of the requested type.  If it doesn't exist or isn't of
			the type, an assertion fires.  Otherwise, defaultValue is 
			returned.
		\return Returns the object found.
	**/
	LuaObject GetTable( const LuaObject& obj, int key, bool require = true );
	LuaObject GetTable( const LuaObject& obj, const char* key, bool require = true );

	void MergeObjects( LuaObject& mergeTo, const LuaObject& mergeFrom, bool replaceDuplicates );

    bool GetBoolean(const LuaObject& obj, int key, bool require, bool defaultValue);
    bool GetBoolean(const LuaObject& obj, const char* key, bool require, bool defaultValue);
    lua_Integer GetInteger(const LuaObject& obj, int key, bool require, int defaultValue);
    lua_Integer GetInteger(const LuaObject& obj, const char* key, bool require, int defaultValue);
    float GetFloat(const LuaObject& obj, int key, bool require, float defaultValue);
    float GetFloat(const LuaObject& obj, const char* key, bool require, float defaultValue);
    void* GetLightUserdata(const LuaObject& obj, int key, bool require, void* defaultValue);
    void* GetLightUserdata(const LuaObject& obj, const char* key, bool require, void* defaultValue);
    const char* GetString(const LuaObject& obj, int key, bool require, const char* defaultValue);
    const char* GetString(const LuaObject& obj, const char* key, bool require, const char* defaultValue);
    LuaObject GetTable(const LuaObject& obj, int key, bool require);
    LuaObject GetTable(const LuaObject& obj, const char* key, bool require);
    void MergeObjects(LuaObject& mergeTo, const LuaObject& mergeFrom, bool replaceDuplicates);

} // namespace LuaHelper


} // namespace LuaPlus


#endif // LUAPLUS__LUAHELPER_OBJECT_H
