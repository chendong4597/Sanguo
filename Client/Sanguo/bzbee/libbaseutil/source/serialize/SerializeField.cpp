/*!
 * \file SerializeField.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 04/11/2016
 *
 * 
 */
#include "SerializeField.h"

NS_BEGIN

FieldDefineBase::FieldDefineBase(ISerializeOp* pOp, const std::type_index& type, const tstring& name, const IFieldAttribute* attr)
	: m_serOperator(pOp)
	, m_typeIndex(type)
	, m_name(name)
	, m_fieldAttribute(attr)
{}

bool SerializeFieldFlag::setter         (uint value) { return dynamic_cast<FieldFlagDefineBase*>(m_pDefine)->setter (m_pThis, value); }
bool SerializeFieldFlag::cleaner        (uint value) { return dynamic_cast<FieldFlagDefineBase*>(m_pDefine)->cleaner(m_pThis, value); }
uint SerializeFieldFlag::getter         (void) const { return dynamic_cast<FieldFlagDefineBase*>(m_pDefine)->getter (m_pThis       ); }
uint SerializeFieldFlag::getDefaultValue(void) const { return dynamic_cast<FieldFlagDefineBase*>(m_pDefine)->m_defValue             ; }

NS_END
