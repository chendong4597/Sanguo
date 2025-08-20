/*!
 * \file ManagedFactory.cpp
 *
 * \author mattho
 * \date 2018/05/09
 *
 *
 */
#include "ManagedFactory.h"

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
MFactory::~MFactory()
{
}

bool MFactory::factoryCanImport(const tstring& inFilename)
{
	return false;
}

MObject* MFactory::factoryCreateFile(MClass* inClass,
	MObject* inOuter, const tstring& inName, ManagedFlags inFlags,
	const tstring& inFilename, const char* inParams, bool& OutOperationCanceled)
{
	return nullptr;
}

MObject* MFactory::importObject(MClass* inClass,
	MObject* inOuter, const tstring& inName, ManagedFlags inFlags, const char* inFilename,
	MObject* inContext, MFactory* inFactory, const char* inParams, bool& OutOperationCanceled)
{
	return nullptr;
}
//////////////////////////////////////////////////////////////////////////

NS_END
