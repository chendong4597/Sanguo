/*!
 * \file ManagedFactory.h
 *
 * \author mattho
 * \date 2018/05/09
 *
 *
 */
#pragma once

#include "ManagedObject.h"
#include <utils/DynamicArray.h>

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
class MFactory : public MObject
{
	DECLARE_ABSTRACT_MCLASS(MFactory, MObject, ManagedClassFlags::None, bzbee);

public:
	MFactory() = default;
	virtual ~MFactory();

	/** Whether the specified file can be imported by this factory. */
	virtual bool factoryCanImport(const tstring& inFilename);

	virtual bool factoryCanCreateNew() const { return m_bIsCreateNew; }

	/** Create a new object by importing it from a file name. */
	virtual MObject* factoryCreateFile(MClass* inClass,
		MObject* inOuter, const tstring& inName, ManagedFlags inFlags,
		const tstring& inFilename, const char* inParams, bool& OutOperationCanceled);

protected:
	/** Create a new object by importing it from a text buffer. */
	virtual MObject* factoryCreateText() { return nullptr; }
	virtual MObject* factoryCreateBinary() { return nullptr; }

public:
	static MObject* importObject(MClass* inClass,
		MObject* inOuter, const tstring& inName, ManagedFlags inFlags, const char* inFilename,
		MObject* inContext, MFactory* inFactory, const char* inParams, bool& OutOperationCanceled);

protected:
	SubclassOf<MObject>		m_supportedClass;
	/** List of formats supported by the factory. Each entry is the file extension. */
	dynamic_array<tstring>	m_formats;

	bool	m_bIsCreateNew{};
	/** true if the factory imports objects from text. */
	bool	m_bIsText{};

	/** Name of the file currently being imported. */
	static tstring	s_filename;
};
//////////////////////////////////////////////////////////////////////////

NS_END
