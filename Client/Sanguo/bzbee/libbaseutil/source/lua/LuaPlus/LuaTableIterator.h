///////////////////////////////////////////////////////////////////////////////
// This source file is part of the LuaPlus source distribution and is Copyright
// 2001-2011 by Joshua C. Jensen (jjensen@workspacewhiz.com).
//
// The latest version may be obtained from http://luaplus.org/.
//
// The code presented in this file may be used in any environment it is
// acceptable to use Lua.
///////////////////////////////////////////////////////////////////////////////
#ifndef LUAPLUS__LUATABLEITERATOR_H
#define LUAPLUS__LUATABLEITERATOR_H

#include "LuaPlusInternal.h"
#include "LuaObject.h"

///////////////////////////////////////////////////////////////////////////////
// namespace LuaPlus
///////////////////////////////////////////////////////////////////////////////
namespace LuaPlus
{

/**
	The LuaTableIterator class provides a far simpler, safer, and more natural
	looking table iteration method.
**/
class LuaTableIterator
{
public:
	LuaTableIterator(const LuaObject& tableObj, bool doReset = true);
	~LuaTableIterator();
	void Reset();
	void Invalidate();
	bool Next();
	bool IsValid() const;
	LuaTableIterator& operator++();
	operator bool() const;
	LuaObject& GetKey();
	LuaObject& GetValue();

protected:

private:
	/**
		Don't allow copies.
	**/
	LuaTableIterator& operator=( const LuaTableIterator& iter );
	LuaTableIterator( const LuaTableIterator& iter );

	LuaObject m_keyObj;
	LuaObject m_valueObj;
	LuaObject m_tableObj;				///< The table object being iterated.
	bool m_isDone;
};

} // namespace LuaPlus

#endif // LUAPLUS__LUATABLEITERATOR_H
