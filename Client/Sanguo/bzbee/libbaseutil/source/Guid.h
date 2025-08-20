/*!
 * \file Guid.h
 *
 * \author mattho
 * \date 2018/05/18
 *
 *
 */
#pragma once

#include "platform/Platform.h"

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
class Guid
{
public:
    operator ref_type<uint32[4]>();

	/** A GUID that has all its components set to zero is considered invalid. */
    bool isValid() const;

    bool operator==(const Guid& inOther) const;

    bool operator!=(const Guid& inOther) const;

    tstring toString() const;

	/** Returns a new Guid. */
    static Guid create();
    static uint32 getTypeHash(const Guid& inKey);

public:
	uint32	data[4]{};

};
//////////////////////////////////////////////////////////////////////////

NS_END

//////////////////////////////////////////////////////////////////////////
namespace std
{
	template<> struct hash<bzbee::Guid>
	{
		FORCEINLINE std::size_t operator()(const bzbee::Guid& inKey) const
		{
			return bzbee::Guid::getTypeHash(inKey);
		}
	};
}
//////////////////////////////////////////////////////////////////////////
