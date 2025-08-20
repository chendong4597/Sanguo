/*!
 * \file ManagedPackage.h
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
class MPackage : public MObject
{
	DECLARE_MCLASS(MPackage, MObject, ManagedClassFlags::None, bzbee);
};
//////////////////////////////////////////////////////////////////////////

NS_END
