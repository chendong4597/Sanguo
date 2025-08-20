/*!
 * \file ManagedClass.h
 *
 * \author mattho
 * \date 2018/04/16
 *
 *
 */
#pragma once

#include "ManagedObject.h"

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
class MField : public MObject
{
	DECLARE_CASTED_ABSTRACT_MCLASS(MField, MObject, ManagedClassFlags::None, ManagedCastFlags::Field, bzbee);
public:
	MField() = default;
	MField(ManagedStaticConstruct, ManagedFlags inFlags);

public:
	MField*		m_next{};
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class MStruct : public MField
{
	DECLARE_CASTED_MCLASS(MStruct, MField, ManagedClassFlags::None, ManagedCastFlags::Struct, bzbee);
public:
	MStruct() = default;
	MStruct(ManagedStaticConstruct, uint32 inSize, ManagedFlags inFlags);

	virtual void setSuperStruct(MStruct* inSuperStruct) { m_superStruct = inSuperStruct; }

private:
	MStruct*	m_superStruct{};

public:
	uint32		m_propertiesSize{};
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class MClass : public MStruct
{
	DECLARE_CASTED_MCLASS(MClass, MStruct, ManagedClassFlags::None, ManagedCastFlags::Class, bzbee);
public:
	typedef void(*InternalConstructorType)(void*);
	typedef void(*InternalDestructorType)(void*);
	typedef void(*StaticConstructorType)(MObject*);
	typedef MClass* (*StaticClassIdFunctionType)();

public:
	MClass() = default;
	MClass(ManagedStaticConstruct, const tstring& inName, uint32 inSize,
		ManagedClassFlags inClassFlags, ManagedCastFlags inCastFlags, ManagedFlags inFlags,
		InternalConstructorType inInternalConstructorFunc, InternalDestructorType inInternalDestructorFunc);

	virtual void setSuperStruct(MStruct* inSuperStruct) override;

public:
	static void createClass(
		MClass*& OutClass,
		uint32 inSize,
		ManagedClassFlags inClassFlags,
		ManagedCastFlags inCastFlags,
		ManagedFlags inFlags,
		const char* inPackageName,
		const char* inName,
		InternalConstructorType	inInternalConstructorFunc,
		InternalDestructorType inInternalDestructorFunc,
		StaticClassIdFunctionType inSuperClassFunc,
		bool bIsDynamic = false);

private:
	ManagedClassFlags			m_classFlags;
	ManagedCastFlags			m_classCastFlags;

	InternalConstructorType		m_fnInternalConstructor{};
	InternalDestructorType		m_fnInternalDestructor{};
};
//////////////////////////////////////////////////////////////////////////

NS_END
