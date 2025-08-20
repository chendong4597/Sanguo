/*!
 * \file SerializeFieldList.h
 *
 * \author
 * \date 2019/09/11
 *
 * 
 */

#pragma once
#include <serialize/SerializeOp.h>
#include <serialize/SerializeField.h>

NS_BEGIN

class Component;
class Rtti;

class SerializeFieldList final
{
public:
    typedef std::vector<FieldDefineBase*> FieldDefineList;

public:
	class const_iterator final
	{
	public:
		const_iterator(void);
		const_iterator(Component* pThis);

		const_iterator& operator++(void);
		inline bool operator!= (const const_iterator& other) const noexcept { return m_pRtti != other.m_pRtti || m_it != other.m_it; }
		inline ISerializeField operator* (void) const { return { m_pThis, *m_it }; }

	private:
		void process_iterator(void);

	private:
		Component* m_pThis;
		const Rtti* m_pRtti;
		FieldDefineList::const_iterator m_it;
	};

public:
	SerializeFieldList(Component* pThis);

	inline const_iterator begin(void) const { return m_pThis; }
	inline const_iterator end  (void) const { return {}     ; }

private:
	Component* const m_pThis;
};

NS_END
