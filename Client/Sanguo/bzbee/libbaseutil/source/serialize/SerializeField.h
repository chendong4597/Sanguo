/*!
 * \file SerializeField.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 04/11/2016
 *
 * 
 */
#pragma once
#include "FieldAttributes.h"
#include <typeindex>

NS_BEGIN

class ISerializeOp;

struct FieldDefineBase
{
	FieldDefineBase(ISerializeOp* pOp, const std::type_index& type, const tstring& name, const IFieldAttribute* attr);
	virtual ~FieldDefineBase(void) = default;

	tstring m_name;
	std::type_index m_typeIndex;
	ISerializeOp* m_serOperator{};
	const IFieldAttribute* m_fieldAttribute{};

	virtual void setDefaultValue(void* pThis) {};
};


struct ISerializeField
{
	inline ISerializeOp*          getOperator(void)       const { return m_pDefine->m_serOperator   ; }
	inline std::type_index        getTypeIndex(void)      const { return m_pDefine->m_typeIndex     ; }
	inline tstring                getName(void)           const { return m_pDefine->m_name          ; }
	inline const IFieldAttribute* getFieldAttribute(void) const { return m_pDefine->m_fieldAttribute; }

	void* m_pThis;
	FieldDefineBase* m_pDefine;
};

template <typename T> struct SerializeField final : public ISerializeField
{
	typedef T Type;
	bool setter(Type value);
	Type getter(void) const;
	Type getDefaultValue(void) const;
};

struct SerializeFieldFlag final : public ISerializeField
{
	bool setter (uint value);
	bool cleaner(uint value);
	uint getter (void) const;
	uint getDefaultValue(void) const;
};
struct SerializeFieldSeparate final : public ISerializeField {};

template <typename T> struct FieldDefineBaseT : public FieldDefineBase
{
	typedef T Type;
	Type m_defValue{};

	FieldDefineBaseT(ISerializeOp* pOp, const tstring& name, const IFieldAttribute* attr)
		: FieldDefineBase(pOp, typeid(Type), name, attr)
	{}

	virtual bool setter(void* pThis, Type value)       = 0;
	virtual Type getter(void* pThis            ) const = 0;

	virtual void setDefaultValue(void* pThis) override { m_defValue = getter(pThis); }
};

template <typename ClassT, typename SetterValueT, typename GetterT>
struct FieldDefine final : public FieldDefineBaseT<std::remove_cv_t<std::remove_reference_t<SetterValueT>>>
{
	typedef std::remove_cv_t<std::remove_reference_t<SetterValueT>> Type;
	typedef bool (ClassT::*SetterT)(SetterValueT);

	FieldDefine(ISerializeOp* pOp, const tstring& name, SetterT s, GetterT g, const IFieldAttribute* attr)
		: FieldDefineBaseT<Type>(pOp, name, attr)
		, m_setter(s)
		, m_getter(g)
	{}

private:
	virtual bool setter(void* pThis, Type value)       override { return           (reinterpret_cast<ClassT*>(pThis)->*m_setter)(value) ; }
	virtual Type getter(void* pThis            ) const override 
	{ 
		return std::move((reinterpret_cast<ClassT*>(pThis)->*m_getter)()); 
	}

	SetterT m_setter;
	GetterT m_getter;
};

template <typename T> bool                             SerializeField<T>::setter         (typename SerializeField<T>::Type value) { return static_cast<FieldDefineBaseT<T>*>(m_pDefine)->setter(m_pThis, value); }
template <typename T> typename SerializeField<T>::Type SerializeField<T>::getter         (void) const                             { return static_cast<FieldDefineBaseT<T>*>(m_pDefine)->getter(m_pThis       ); }
template <typename T> typename SerializeField<T>::Type SerializeField<T>::getDefaultValue(void) const                             { return static_cast<FieldDefineBaseT<T>*>(m_pDefine)->m_defValue            ; }


struct FieldFlagDefineBase : public FieldDefineBase
{
	FieldFlagDefineBase(ISerializeOp* ser, const tstring& name, const IFieldAttribute* attr)
		: FieldDefineBase(ser, typeid(SerializeFieldFlag), name, attr)
	{}

	virtual bool setter (void* pThis, uint value)       = 0;
	virtual bool cleaner(void* pThis, uint value)       = 0;
	virtual uint getter (void* pThis            ) const = 0;

	uint m_defValue{};

	virtual void setDefaultValue(void* pThis) override { m_defValue = getter(pThis); }
};

template <typename ClassT> class FieldFlagDefine final : public FieldFlagDefineBase
{
public:
	typedef bool (ClassT::*SetterT )(uint);
	typedef bool (ClassT::*CleanerT)(uint);
	typedef uint (ClassT::*GetterT )(void) const;

public:
	FieldFlagDefine(ISerializeOp* ser, const tstring& name, SetterT s, CleanerT c, GetterT g, const IFieldAttribute* attr)
		: FieldFlagDefineBase(ser, name, attr)
		, m_setter (s)
		, m_cleaner(c)
		, m_getter (g)
	{}

private:
	virtual bool setter (void* pThis, uint value)       override { return           (reinterpret_cast<ClassT*>(pThis)->*m_setter )(value) ; }
	virtual bool cleaner(void* pThis, uint value)       override { return           (reinterpret_cast<ClassT*>(pThis)->*m_cleaner)(value) ; }
	virtual uint getter (void* pThis            ) const override 
	{
		return std::move((reinterpret_cast<ClassT*>(pThis)->*m_getter )()); 
	}

	SetterT  m_setter;
	CleanerT m_cleaner;
	GetterT  m_getter;
};

struct FieldSeparateDefine final : public FieldDefineBase
{
	FieldSeparateDefine(ISerializeOp *ser, const tstring& name, const IFieldAttribute* attr)
		: FieldDefineBase(ser, typeid(SerializeFieldSeparate), name, attr)
	{}
};

NS_END
