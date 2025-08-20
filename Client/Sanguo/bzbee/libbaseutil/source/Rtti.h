/*!
 * \file Rtti.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 03/15/2016
 *
 * 
 */
#pragma once
#include "BaseType.h"
#include <typeindex>
#include <memory>

namespace LuaPlus
{
    class LuaObject;
}

NS_BEGIN

class ISerializeOp;
struct FieldDefineBase;
typedef std::vector<FieldDefineBase*> FieldDefineList;

class Rtti final
{
public:
	Rtti(const std::type_index& typeIndex, const char* pszClassName, const Rtti* pBaseRtti);
	~Rtti(void);

	const std::type_index& getTypeIndex(void) const { return m_typeIndex; }
	const tstring& getClassName(void) const { return m_className; }
	const Rtti* getBaseRtti (void) const { return m_pBaseRtti; }

	bool isSame(const std::type_index& typeIndex) const { return (m_typeIndex == typeIndex); }
	bool isDerivedFrom(const std::type_index& baseTypeIndex) const;
	bool isDerivedFrom(const tstring& baseTypeName) const;

public:
	template <typename T, typename... Args>
	T* addField(ISerializeOp* pOp, const tstring& name, Args... args) const
	{
		if (!m_fieldNameSet.insert(name).second) return nullptr;

		T* pT = new T(pOp, name, args...);
		m_fieldDefineList.push_back(pT);
		return pT;
	}

	inline FieldDefineList& getFieldDefineList(void) const { return m_fieldDefineList; }

private:
	std::type_index m_typeIndex;
	tstring m_className;
	const Rtti* m_pBaseRtti{};

	mutable FieldDefineList m_fieldDefineList;
	mutable std::set<tstring> m_fieldNameSet;
};

NS_END

#define RTTI_ROOT(this_class) protected: typedef this_class Self; \
    public: mutable std::shared_ptr<LuaPlus::LuaObject> __lua_p_object; \
	public: static const NS::Rtti* __RttiData() { static const NS::Rtti s(std::type_index(typeid(Self)), #this_class, nullptr); return &s; } \
	virtual const NS::Rtti* getRtti() const { return Self::__RttiData(); }

#define RTTI_DEF(this_class) private: typedef Self Super; \
    protected: typedef this_class Self; \
	public: static const NS::Rtti* __RttiData() { static const NS::Rtti s(std::type_index(typeid(Self)), #this_class, Super::__RttiData()); return &s; } \
	virtual const NS::Rtti* getRtti() const override { return Self::__RttiData(); }

#define RTTI_DEF_UNIVERSAL(this_class, super_class) protected: typedef this_class Self; private: using Super = typename super_class::Self; \
	public: static const NS::Rtti* __RttiData() { static const NS::Rtti s(std::type_index(typeid(Self)), #this_class, Super::__RttiData()); return &s; } \
	virtual const NS::Rtti* getRtti() const override { return Self::__RttiData(); }
