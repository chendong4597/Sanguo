/*!
 * \file ManagedInitializer.h
 *
 * \author mattho
 * \date 2018/05/10
 *
 *
 */
#pragma once

#include "ManagedObjectUtils.h"

NS_BEGIN

class ManagedInstancingGraph;

//////////////////////////////////////////////////////////////////////////
class ManagedInitializer
{
public:
	ManagedInitializer();
	ManagedInitializer(MObject* inTarget, MObject* inPrefab,
		bool bShouldInitializeProps, ManagedInstancingGraph* inInstanceGraph = nullptr);
	~ManagedInitializer();

	friend class MObject;

private:
	/** Object to initialize, from static allocate object, after construction */
	MObject*	m_target{};
};
//////////////////////////////////////////////////////////////////////////

NS_END
