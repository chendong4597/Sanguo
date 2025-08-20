/*!
 * \file SerializeFieldList.cpp
 *
 * \author 
 * \date 2019/09/11
 *
 * 
 */
#include "SerializeFieldList.h"
#include "Component.h"

NS_BEGIN

static FieldDefineList::const_iterator getEnd(void)
{
	static FieldDefineList s;
	return s.end();
}

SerializeFieldList::const_iterator::const_iterator(void)
	: m_pThis{}
	, m_pRtti{}
{
	m_it = getEnd();
}


SerializeFieldList::const_iterator::const_iterator(Component* pThis)
	: m_pThis(pThis)
	, m_pRtti(pThis->getRtti())
{
	while (auto pRootRtti = m_pRtti->getBaseRtti()) m_pRtti = pRootRtti;
	m_it = m_pRtti->getFieldDefineList().begin();
	process_iterator();
}


SerializeFieldList::const_iterator& SerializeFieldList::const_iterator::operator++(void)
{
	++m_it;
	process_iterator();
	return *this;
}


void SerializeFieldList::const_iterator::process_iterator(void)
{
	while (m_it == m_pRtti->getFieldDefineList().end())
	{
		auto pDerived = static_cast<Component*>(m_pThis)->getRtti();
		if (m_pRtti == pDerived)
		{
			m_pRtti = nullptr;
			m_it = getEnd();
			break;
		}

		while (m_pRtti != pDerived->getBaseRtti()) pDerived = pDerived->getBaseRtti();
		m_pRtti = pDerived;
		m_it = m_pRtti->getFieldDefineList().begin();
	}
}


SerializeFieldList::SerializeFieldList(Component* pThis)
	: m_pThis(pThis)
{}

NS_END
