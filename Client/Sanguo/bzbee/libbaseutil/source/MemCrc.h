/*!
 * \file MemCrc.h
 *
 * \author mattho
 * \date 2016/10/12
 *
 *
 */
#pragma once

#include "BaseType.h"

NS_BEGIN

uint32 memcrc32(const void* inData, uint32 inLength, uint32 inCrc = 0);

uint32 strihash(const char* inData);
uint32 strcrc32(const char* inData, uint32 inCrc = 0);

NS_END
