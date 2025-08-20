/*!
 * \file ManagedObjectUtils.h
 *
 * \author mattho
 * \date 2016/11/17
 *
 *
 */
#pragma once

#include <BaseType.h>
#include <EnumUtil.h>
#include <AssertionMacros.h>
#include <memory>

NS_BEGIN

class MObject;
class MClass;

//////////////////////////////////////////////////////////////////////////
enum class ManagedFlags : uint32
{
	None = 0,

	// Object is visible outside its package.
	Public = 0x00000001,

	// This object is its class's default object
	ClassDefaultObject = 0x00000010,
	// This object is a template for another object - treat like a class default object
	ArchetypeObject = 0x00000020,
	// Don't save object
	Transient = 0x00000040,

	//// The group of flags tracks the stages of the lifetime of an object
	NeedPostLoad = 0x00001000,
	NeedPostLoadSubobjects = 0x00002000,
	BeginDestroyed = 0x00008000,
	FinishDestroyed = 0x00010000,

	ALL_FLAGS = 0x0007ffff
};
ENUM_CLASS_FLAGS(ManagedFlags);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
enum class ManagedInternalFlags : uint32
{
	None = 0,
};
ENUM_CLASS_FLAGS(ManagedInternalFlags);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
enum class ManagedClassFlags : uint32
{
	None = 0,
	Abstract = 0x00000001,
};
ENUM_CLASS_FLAGS(ManagedClassFlags);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
enum class ManagedCastFlags : UI64
{
	None = 0,
	Field = 0x0000000000000001,
	Struct = 0x0000000000000008,
	Class = 0x0000000000000020,
	Package = 0x0000000400000000,
};
ENUM_CLASS_FLAGS(ManagedCastFlags);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
enum class ManagedLoadFlags : uint32
{
	None = 0,
};
ENUM_CLASS_FLAGS(ManagedLoadFlags);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
enum ManagedStaticConstruct { StaticConstructor };
enum ManagedInternalConstruct { InternalUseOnlyConstructor };

#define DECLARE_BASE_MCLASS(TClass, TSuperClass, TClassFlags, TCastFlags, TPackage) \
private: \
	static bzbee::MClass*	s_privateClassId; \
	static void createPrivateClassId_##TClass(); \
	typedef TSuperClass	SuperClass; \
	static FORCEINLINE bzbee::ManagedClassFlags getClassFlags() \
		{ return TClassFlags; } \
	static FORCEINLINE bzbee::ManagedCastFlags getCastFlags() \
		{ return TCastFlags; } \
public: \
	RTTI_DEF(TClass); \
	static bzbee::MClass* classId() \
		{ \
			if (!s_privateClassId) \
			{ \
				createPrivateClassId_##TClass(); \
			} \
			return s_privateClassId; \
		} \
	static FORCEINLINE const char* getPackageName() \
		{ return #TPackage; }
//	void* operator new(std::size_t nSize, \
//		MObject* inOuter = (MObject*)getTransientPackage(), \
//		HashName inName = NAME_None, ManagedFlags inFlags = ManagedFlags::None) \
//		{ return allocateObject(classId(), inOuter, inName, inFlags); } \
//	void* operator new(std::size_t nSize, ManagedInternalConstruct* inPtr) \
//		{ return (void*)inPtr; } \
//	void operator delete(void*, MObject*, HashName, ManagedFlags) { } \
//	void operator delete(void*, ManagedInternalConstruct*) { } \
//	void operator delete(void*) { }

#define DECLARE_ABSTRACT_MCLASS(TClass, TSuperClass, TClassFlags, TPackage) \
	DECLARE_BASE_MCLASS(TClass, TSuperClass, (bzbee::ManagedClassFlags)(TClassFlags) | bzbee::ManagedClassFlags::Abstract, bzbee::ManagedCastFlags::None, TPackage)

#define DECLARE_CASTED_ABSTRACT_MCLASS(TClass, TSuperClass, TClassFlags, TCastFlags, TPackage) \
	DECLARE_BASE_MCLASS(TClass, TSuperClass, (bzbee::ManagedClassFlags)(TClassFlags) | bzbee::ManagedClassFlags::Abstract, (bzbee::ManagedCastFlags)TCastFlags, TPackage)

#define DECLARE_CASTED_MCLASS(TClass, TSuperClass, TClassFlags, TCastFlags, TPackage) \
	DECLARE_BASE_MCLASS(TClass, TSuperClass, (bzbee::ManagedClassFlags)(TClassFlags), (bzbee::ManagedCastFlags)TCastFlags, TPackage)
	
#define DECLARE_MCLASS(TClass, TSuperClass, TClassFlags, TPackage) \
	DECLARE_BASE_MCLASS(TClass, TSuperClass, (bzbee::ManagedClassFlags)(TClassFlags), bzbee::ManagedCastFlags::None, TPackage)

#define IMPLEMENT_MCLASS(TClass)	\
	MClass*	TClass::s_privateClassId = nullptr; \
	void TClass::createPrivateClassId_##TClass() \
	{ \
		MClass::createClass \
		( \
			TClass::s_privateClassId, \
			sizeof(TClass), \
			ManagedClassFlags::None, \
			ManagedCastFlags::None, \
			ManagedFlags::Public | ManagedFlags::Transient, \
			(#TClass) + 1, \
			TClass::getPackageName(), \
			&TClass::internalConstructor, \
			&TClass::internalDestructor, \
			&TClass::SuperClass::classId, \
			false \
		); \
	}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class MClass;

template<typename ObjectType, typename = typename
	std::enable_if<std::is_base_of<MObject, typename std::decay<ObjectType>::type>::value>::type>
class SubclassOf
{
public:

private:
	MClass*	m_class{};
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/** Shared pointer for ManagedObject. */
template<typename ObjectType>
using shared_mptr = std::shared_ptr<typename std::decay<ObjectType>::type>;

/** Weak pointer for ManagedObject. */
template<typename ObjectType>
class weak_mptr
{
public:
	weak_mptr(ObjectType* inPtr)
		: m_ptr(inPtr)
	{}

	weak_mptr(const weak_mptr& inOther)
		: m_ptr(inOther.m_ptr)
	{}

	FORCEINLINE ObjectType* get() const
	{
		return m_ptr;
	}

	FORCEINLINE bool operator==(const weak_mptr& inOther) const
	{
		return m_ptr == inOther.m_ptr;
	}

private:
	ObjectType*	m_ptr;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
namespace ObjectCore
{
	void initialize();
	void terminate();
};
//////////////////////////////////////////////////////////////////////////

NS_END

//////////////////////////////////////////////////////////////////////////
namespace std
{
	template<typename T>
	struct hash<bzbee::weak_mptr<T>>
	{
		typedef bzbee::weak_mptr<T> argument_type;
		typedef std::size_t result_type;

		FORCEINLINE result_type operator()(const argument_type& s) const
		{
			return std::hash<T*>{}(s.get());
		}
	};
}
//////////////////////////////////////////////////////////////////////////
