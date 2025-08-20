/*!
* \file RandomDataMgr.cpp
*
* \author ChenBK
* \date 2018/05/25
*
*
*/
#include "stdafx.h"
#include "RandomData.h"
#include <Guid.h>
#include <MemCrc.h>
#include "BaseType.h"

using namespace NS;

RandomDataMgr& RandomDataMgr::getInstance()
{
	static RandomDataMgr s_DropDataMgr;
	return s_DropDataMgr;
}

NS::I64 RandomDataMgr::CreateGuidInt64()
{
	NS::I64 retUuid{};
	Guid guid = Guid::create();

	if (guid.isValid())
	{
		while (true)
		{
			uint32 hTemp = memcrc32(guid.data, sizeof(uint32) * 2);
			uint32 lTemp = memcrc32(guid.data + 2, sizeof(uint32) * 2);

			retUuid = ((NS::I64)hTemp << 32) | (NS::I64)lTemp;
			if (retUuid != 0)
			{
				break;
			}
		}
	}

	return retUuid;
}