/*!
 * \file ManagedObject.cpp
 *
 * \author mattho
 * \date 2016/09/29
 *
 *
 */
#include "ManagedObject.h"
#include "ManagedClass.h"
#include <utils/ContainerMemory.h>

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_MCLASS(MObject);
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
bool	MObject::s_bIsInitialized = false;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//// ManagedObject
MObject::MObject()
{
}

MObject::MObject(const ManagedInitializer& inInitializer)
{
	ASSERT(inInitializer.m_target != nullptr && inInitializer.m_target != this);
	const_cast<ManagedInitializer&>(inInitializer).m_target = this;
}

MObject::MObject(ManagedStaticConstruct, ManagedFlags inFlags)
	: m_flags(inFlags)
{
}

MObject::~MObject()
{
	ASSERT(hasAnyFlags(ManagedFlags::FinishDestroyed));
}

const char* MObject::getDebugName() const
{
	return getRtti()->getClassName().c_str();
}

bool MObject::initialize(Component* pEntity)
{
	conditionalPostLoad();
	return true;
}

void MObject::terminate()
{
	conditionalBeginDestroy();
	conditionalFinishDestroy();
}

void MObject::postInitProperties()
{
}

void MObject::postLoad()
{
}

void MObject::postLoadSubobjects(ManagedInstancingGraph* inOuterInstanceGraph)
{
	clearFlags(ManagedFlags::NeedPostLoadSubobjects);
}

// #ifdef _DEBUG
// static std::vector<ManagedObject*>		s_debugBeginDestroyed;
// static std::vector<ManagedObject*>		s_debugFinishDestroyed;
// #endif

void MObject::beginDestroy()
{
	ASSERT_LOG(hasAnyFlags(ManagedFlags::BeginDestroyed),
		"Trying to call ManagedObject::beginDestroy from outside of ManagedObject::conditionalBeginDestroy on object {0}. Please fix up the calling code.",
		getDebugName());

// #ifdef _DEBUG
// 	s_debugBeginDestroyed.removeSingleSwap(this);
// #endif
}

void MObject::finishDestroy()
{
	ASSERT_LOG(hasAnyFlags(ManagedFlags::FinishDestroyed),
		"Trying to call ManagedObject::finishDestroy from outside of ManagedObject::conditionalFinishDestroy on object {0}. Please fix up the calling code.",
		getDebugName());

// #ifdef _DEBUG
// 	s_debugFinishDestroyed.removeSingleSwap(this);
// #endif
}

void MObject::conditionalPostLoad()
{
	if (hasAnyFlags(ManagedFlags::NeedPostLoad))
	{
		clearFlags(ManagedFlags::NeedPostLoad);

		MObject* archetype = getArchetype();
		if (archetype) archetype->conditionalPostLoad();

		conditionalPostLoadSubobjects();
		postLoad();
	}
}

void MObject::conditionalPostLoadSubobjects(ManagedInstancingGraph* inOuterInstanceGraph)
{
	if (hasAnyFlags(ManagedFlags::NeedPostLoadSubobjects))
	{
		postLoadSubobjects(inOuterInstanceGraph);
	}
}

bool MObject::conditionalBeginDestroy()
{
	if (!hasAnyFlags(ManagedFlags::BeginDestroyed))
	{
		setFlags(ManagedFlags::BeginDestroyed);
// #ifdef _DEBUG
// 		ASSERT(!s_debugBeginDestroyed.find(this).second);
// 		s_debugBeginDestroyed.emplace_back(this);
// #endif

		beginDestroy();

// #ifdef _DEBUG
// 		ASSERT_LOG(!s_debugBeginDestroyed.find(this).second, "{0} failed to route beginDestroy", getDebugName());
// #endif
		return true;
	}
	return false;
}

bool MObject::conditionalFinishDestroy()
{
	if (!hasAnyFlags(ManagedFlags::FinishDestroyed))
	{
		setFlags(ManagedFlags::FinishDestroyed);
// #ifdef _DEBUG
// 		ASSERT(!s_debugFinishDestroyed.find(this).second);
// 		s_debugFinishDestroyed.emplace_back(this);
// #endif

		finishDestroy();

// #ifdef _DEBUG
// 		ASSERT_LOG(!s_debugFinishDestroyed.find(this).second, "{0} failed to route finishDestroy", getDebugName());
// #endif
		return true;
	}
	return false;
}

MObject* MObject::constructObject(MObject* inObject, MObject* inOuter, const tstring& inName)
{
	inObject->m_outer = inOuter;
	inObject->m_name = inName;
	inObject->setFlags(ManagedFlags::NeedPostLoad);
	inObject->postInitProperties();
	return inObject;
}

void MObject::destroyObject(MObject* inObject)
{
	inObject->conditionalBeginDestroy();
	inObject->conditionalFinishDestroy();
	delete inObject;
}

shared_mptr<MObject> MObject::makeSharedObject(MObject* inObject)
{
	if (!inObject)
	{
		return std::shared_ptr<MObject>();
	}

	if (inObject->m_weakptr.expired())
	{
		const auto sharedptr = std::shared_ptr<MObject>(inObject,
			[](MObject* inRawPtr)
		{
			bzbee::delete_mobject(inRawPtr);
		});
		inObject->m_weakptr = sharedptr;
		return sharedptr;
	}
	else
	{
		return std::shared_ptr<MObject>(inObject->m_weakptr);
	}
}

void MObject::registerObject(const char* inPackageName, const char* inName)
{
}

MObject* MObject::findObjectFastInternal(MClass* inClass,
	MObject* inOuter, const tstring& inName, bool bExactClass, bool bAnyPackage, ManagedFlags inExclusiveFlags)
{
	return nullptr;
}

MObject* MObject::allocateObject(MClass* inClass, MObject* inOuter, const tstring& inName, ManagedFlags uFlags,
	MObject* inArchetype, MObject* inPlace, MObject* inSubobjectRoot, ManagedInstancingGraph* inInstanceGraph)
{
	return nullptr;
}

MObject* MObject::allocateObject(uint32 inSize, uint32 inAlignment)
{
	const uint32 AllocateSize = bzbee::align(inSize, inAlignment);
	return (MObject*)std::malloc(AllocateSize);
}

MObject* MObject::loadObject(MClass* inClass, MObject* inOuter, const tstring& inName, ManagedLoadFlags inLoadFlags)
{
	return nullptr;
}

void MObject::initializePrivateClassId(
	MClass* inSuperClass, MClass* inPrivateClass, const char* inPackageName, const char* inName)
{
	ASSERT(inSuperClass != nullptr);
	if (inPrivateClass != inSuperClass)
	{
		inPrivateClass->setSuperStruct(inSuperClass);
	}
	else
	{
		ASSERT(inPrivateClass == MObject::classId());
		inPrivateClass->setSuperStruct(nullptr);
	}

	inPrivateClass->m_class = MClass::classId();
	inPrivateClass->registerObject(inPackageName, inName);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void ObjectCore::initialize()
{
}

void ObjectCore::terminate()
{
}
//////////////////////////////////////////////////////////////////////////

NS_END
