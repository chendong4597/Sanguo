/*!
 * \file Rtti.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 03/15/2016 11:10:19
 *
 * 
 */
#include "Rtti.h"
#include "serialize/SerializeField.h"

NS_BEGIN


Rtti::Rtti(const std::type_index& typeIndex, const char* pszClassName, const Rtti* pBaseRtti)
	: m_typeIndex(typeIndex)
	, m_className(pszClassName)
	, m_pBaseRtti(pBaseRtti)
{}


Rtti::~Rtti(void)
{
	for (auto& pField : m_fieldDefineList) delete pField;
}


bool Rtti::isDerivedFrom(const std::type_index& baseTypeIndex) const
{
	auto pRtti = this;
	do
	{
		if (pRtti->getTypeIndex() == baseTypeIndex) return true;
		pRtti = pRtti->getBaseRtti();
	} while (pRtti);
	return false;
}


bool Rtti::isDerivedFrom(const tstring& baseTypeName) const
{
	auto pRtti = this;
	do
	{
		if (pRtti->getClassName() == baseTypeName) return true;
		pRtti = pRtti->getBaseRtti();
	} while (pRtti);
	return false;
}

NS_END
