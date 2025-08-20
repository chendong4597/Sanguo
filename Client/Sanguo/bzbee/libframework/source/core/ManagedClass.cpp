/*!
 * \file ManagedClass.cpp
 *
 * \author mattho
 * \date 2018/04/16
 *
 *
 */
#include "ManagedClass.h"

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_MCLASS(MField);
IMPLEMENT_MCLASS(MStruct);
IMPLEMENT_MCLASS(MClass);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
MField::MField(ManagedStaticConstruct, ManagedFlags inFlags)
	: MObject(ManagedStaticConstruct::StaticConstructor, inFlags)
{
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
MStruct::MStruct(ManagedStaticConstruct, uint32 inSize, ManagedFlags inFlags)
	: MField(ManagedStaticConstruct::StaticConstructor, inFlags)
	, m_propertiesSize(inSize)
{
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
MClass::MClass(ManagedStaticConstruct, const tstring& inName, uint32 inSize,
	ManagedClassFlags inClassFlags, ManagedCastFlags inCastFlags, ManagedFlags inFlags,
	InternalConstructorType inInternalConstructorFunc, InternalDestructorType inInternalDestructorFunc)
	: MStruct(ManagedStaticConstruct::StaticConstructor, inSize, inFlags)
	, m_classFlags(inClassFlags)
	, m_classCastFlags(inCastFlags)
	, m_fnInternalConstructor(inInternalConstructorFunc)
	, m_fnInternalDestructor(inInternalDestructorFunc)
{
}

void MClass::setSuperStruct(MStruct* inSuperStruct)
{
	Super::setSuperStruct(inSuperStruct);
}

void MClass::createClass(
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
	bool bIsDynamic)
{
	if (!bIsDynamic)
	{
		MObject* const NewObject =
			MObject::allocateObject(sizeof(MClass), std::alignment_of<MClass>::value);
		OutClass = ::new(NewObject) MClass(
			ManagedStaticConstruct::StaticConstructor,
			inName, inSize,
			inClassFlags, inCastFlags, inFlags,
			inInternalConstructorFunc, inInternalDestructorFunc);
		ASSERT(OutClass != nullptr);
	}
	else
	{
		ASSERT_NOT_IMPLEMENTED;
	}

	MObject::initializePrivateClassId(inSuperClassFunc(), OutClass, inPackageName, inName);
}
//////////////////////////////////////////////////////////////////////////

NS_END
