/*!
 * \file ManagedObject.h
 *
 * \author mattho
 * \date 2016/09/29
 *
 *
 */
#pragma once

#include "ManagedInitializer.h"
#include "../component/Component.h"

NS_BEGIN

class MClass;
class MPackage;

//////////////////////////////////////////////////////////////////////////
class MObject : public Component
{
	DECLARE_BASE_MCLASS(MObject, MObject, ManagedClassFlags::None, ManagedCastFlags::None, bzbee)

public:
	MObject();
	MObject(const ManagedInitializer& inInitializer);
	virtual ~MObject();

	FORCEINLINE ManagedFlags getFlags() const { return m_flags; }
	FORCEINLINE bool hasAnyFlags(ManagedFlags inCheckFlags) const { return !!(m_flags & inCheckFlags); }

	FORCEINLINE void setFlags(ManagedFlags inSetFlags) { replaceFlags(m_flags | inSetFlags); }
	FORCEINLINE void clearFlags(ManagedFlags inClearFlags) { replaceFlags(m_flags & ~inClearFlags); }

	FORCEINLINE MObject* getArchetype() const { return m_archetype; }

	FORCEINLINE MObject* getOuter() const { return m_outer; }
	template<typename T> T* getTypedOuter() const
	{
		const std::type_index baseTypeIndex = typeid(T);
		for (const MObject* outer = getOuter(); outer; outer = outer->getOuter())
		{
			if (outer->getRtti()->isDerivedFrom(baseTypeIndex)) return (T*)outer;
		}
		return nullptr;
	}

	FORCEINLINE MClass* getClass() const { return m_class; }

    FORCEINLINE void appendName(tstring& OutName) const { OutName += m_name;; }
	FORCEINLINE void getName(tstring& OutName) const { OutName = m_name; }
	FORCEINLINE const tstring& getName() const { return m_name; }
	const char* getDebugName() const;

	/** Checks if the object is unreachable. */
	FORCEINLINE bool isUnreachable() const { return false; }

	void conditionalPostLoad();
	void conditionalPostLoadSubobjects(ManagedInstancingGraph* inOuterInstanceGraph = nullptr);

	bool conditionalBeginDestroy();
	bool conditionalFinishDestroy();

protected:
	MObject(ManagedStaticConstruct, ManagedFlags inFlags);
	void registerObject(const char* inPackageName, const char* inName);

public:
	static bool FORCEINLINE isInitialized() { return s_bIsInitialized; }

	static MObject* constructObject(MObject* inObject, MObject* inOuter, const tstring& inName);
	static void destroyObject(MObject* inObject);

	static shared_mptr<MObject> makeSharedObject(MObject* inObject);
	static FORCEINLINE MPackage* getTransientPackage() { return staticTransientPackage(); }

	static MObject* loadObject(MClass* inClass, MObject* inOuter, const tstring& inName, ManagedLoadFlags inLoadFlags);

	static FORCEINLINE MObject* findObjectFast(MClass* inClass, MObject* inOuter, const tstring& inName,
		bool bExactClass = false, bool bAnyPackage = false, ManagedFlags inExclusiveFlags = ManagedFlags::None)
	{
		return findObjectFastInternal(inClass, inOuter, inName, bExactClass, bAnyPackage, inExclusiveFlags);
	}

	static FORCEINLINE MObject* findObject(MClass* inClass, MObject* inOuter, const tstring& inName, bool bExactClass = false)
	{
		return nullptr;
	}

protected:
	static MObject* findObjectFastInternal(MClass* inClass, MObject* inOuter, const tstring& inName,
		bool bExactClass = false, bool bAnyPackage = false, ManagedFlags inExclusiveFlags = ManagedFlags::None);

	static MObject* allocateObject(MClass* inClass,
		MObject* inOuter = (MObject*)getTransientPackage(), const tstring& inName = EMPTY_STRING, ManagedFlags uFlags = ManagedFlags::None,
		MObject* inArchetype = nullptr, MObject* inPlacehold = nullptr, MObject* inSubobjectRoot = nullptr, ManagedInstancingGraph* inInstanceGraph = nullptr);
	static MObject* allocateObject(uint32 inSize, uint32 inAlignment);

	static void initializePrivateClassId(
		MClass* inSuperClass, MClass* inPrivateClass, const char* inPackageName, const char* inName);

	static FORCEINLINE void internalConstructor(void* inObject)
	{
		new((ManagedInternalConstruct*)inObject) MObject();
	}
	static FORCEINLINE void internalDestructor(void* inObject)
	{
		((MObject*)inObject)->~MObject();
	}

private:
	static FORCEINLINE MPackage*& staticTransientPackage()
	{
		static MPackage* s_TransientPackage = nullptr;
		return s_TransientPackage;
	}

protected:
	FORCEINLINE void replaceFlags(ManagedFlags inNewFlags)
	{
		ASSERT_LOG(!(inNewFlags & ~ManagedFlags::ALL_FLAGS),
			"{} flagged as {:#x} but is trying to set flags to RuntimeFlags::ALL_FLAGS",
			getDebugName(), enumInt(inNewFlags));
		m_flags = inNewFlags;
	}

	/**
	 * Called after the C++ constructor and after the properties have been initialized, including those loaded from config.
	 * Mainly this is to emulate some behavior of when the constructor was called after the properties were initialized.
	 */
	virtual void postInitProperties();

	/** Do any object-specific cleanup required immediately after loading an object. */
	virtual void postLoad();
	virtual void postLoadSubobjects(ManagedInstancingGraph* inOuterInstanceGraph);

	virtual bool isPostLoadThreadSafe() const { return false; }

	/**
	 * Called before destroying the object.  This is called immediately upon deciding to destroy the object, to allow the object to begin an
	 * asynchronous cleanup process.
	 */
	virtual void beginDestroy();

	/**
	 * Called to check if the object is ready for finishDestroy.  This is called after beginDestroy to check the completion of the
	 * potentially asynchronous object cleanup.
	 *
	 * \return True if the object's asynchronous cleanup has completed and it is ready for finishDestroy to be called.
	 */
	virtual bool isReadyForFinishDestroy() { return true; }

	/**
	 * Called to finish destroying the object.  After ManagedObject::finishDestroy is called, the object's memory should no longer be accessed.
	 *
	 * note: because properties are destroyed here, Super::finishDestroy() should always be called at the end of your child class's
	 * finishDestroy() method, rather than at the beginning.
	 */
	virtual void finishDestroy();

private:
	//// Component interface
	virtual bool initialize(Component* pEntity) override final;
	virtual void terminate() override final;

private:
	union
	{
		UI64		m_uid{};

		struct
		{
			/** Index into global object array... very private. */
			uint32	m_objectIndex;
			/** Global object array indexed slot's version. */
			uint32	m_objectVersion;
		};
	};

	static bool	s_bIsInitialized;

private:
	/** Flags used to track and report various object states. This needs to be 8 byte aligned on 32-bit platforms to reduce memory waste */
	ManagedFlags	m_flags{ ManagedFlags::None };

	/** ManagedObject this object resides in */
	MObject*	m_outer{};

	/** Class the object belongs to */
	MClass*		m_class{};
	/** Name of this object */
	tstring 	m_name;

	MObject*	m_archetype{};

	mutable std::weak_ptr<MObject>	m_weakptr;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename ObjectType>
inline typename std::enable_if <
	std::is_base_of<MObject, typename std::decay<ObjectType>::type>::value,
	typename std::decay<ObjectType>::type*
>::type new_mobject(MObject* inOuter = nullptr, const tstring& inName = EMPTY_STRING, MObject* inPrefab = nullptr)
{
	typedef typename std::decay<ObjectType>::type	ConstructType;
	ConstructType* mObject = new ConstructType();
	return static_cast<ConstructType*>(MObject::constructObject(mObject, inOuter, inName));
}

template<typename ObjectType>
inline typename std::enable_if<
	std::is_base_of<MObject, typename std::decay<ObjectType>::type>::value
>::type delete_mobject(ObjectType*& inObject)
{
	if (inObject)
	{
		inObject->removeFromParent(false);

		MObject::destroyObject(inObject);
		inObject = nullptr;
	}
}

template<typename ObjectType>
FORCEINLINE typename std::enable_if<
	std::is_base_of<MObject, typename std::decay<ObjectType>::type>::value,
	ObjectType*
>::type find_mobject(const tstring& inName, bool bExactClass = false)
{
	//return (ObjectType*)MObject::findObject();
	//return std::decay<ObjectType>::type::deprecatedFindObject(inName, inLoadFlags);
	return nullptr;
}

template<typename ObjectType>
FORCEINLINE typename std::enable_if<
	std::is_base_of<MObject, typename std::decay<ObjectType>::type>::value,
	ObjectType*
>::type load_mobject(const tstring& inName, ManagedLoadFlags inLoadFlags = ManagedLoadFlags::None)
{
	//return (ObjectType*)MObject::findObject();
	return std::decay<ObjectType>::type::deprecatedLoadObject(inName, inLoadFlags);
}

template<typename ObjectType>
FORCEINLINE typename std::enable_if<
	std::is_base_of<MObject, typename std::decay<ObjectType>::type>::value,
	shared_mptr<ObjectType>
>::type shared_mobject(ObjectType* inRawPtr)
{
	return std::static_pointer_cast<ObjectType>(MObject::makeSharedObject(inRawPtr));
}
//////////////////////////////////////////////////////////////////////////

NS_END

using bzbee::new_mobject;
using bzbee::delete_mobject;
using bzbee::shared_mobject;
using bzbee::find_mobject;
