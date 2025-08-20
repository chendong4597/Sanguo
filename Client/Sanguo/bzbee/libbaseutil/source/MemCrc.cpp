/*!
 * \file MemCrc.cpp
 *
 * \author mattho
 * \date 2016/10/12
 *
 *
 */
#include "MemCrc.h"
#include "crc_8x256_tables.h"

// Based on the Slicing-by-8 implementation found here:
// http://slicing-by-8.sourceforge.net/

NS_BEGIN

/**
 *
 * Routine Description:
 *
 * Computes the CRC32c checksum for the specified buffer.                      
 *
 * Arguments:
 *
 *		p_running_crc - pointer to the initial or final remainder value 
 *						used in CRC computations. It should be set to 
 *						non-NULL if the mode argument is equal to CONT or END
 *		p_buf - the packet buffer where crc computations are being performed
 *		length - the length of p_buf in bytes
 *		init_bytes - the number of initial bytes that need to be procesed before
 *					 aligning p_buf to multiples of 4 bytes
 *		mode - can be any of the following: BEGIN, CONT, END, BODY, ALIGN 
 *
 * Return value:
 *		
 *		The computed CRC32c value
 */

uint32_t
static crc32c(
	uint32_t		p_running_crc,
    const uint8_t*	p_buf,
    const uint32_t	length) 
{
	uint32_t crc = ~p_running_crc;
    const uint8_t* p_end = p_buf + length;
	
	while(p_buf < p_end )
		crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
	
	return ~crc;
}


/**
 *
 * Routine Description:
 *
 * Computes the CRC32c checksum for the specified buffer using the slicing by 8 
 * algorithm over 64 bit quantities.                      
 *
 * Arguments:
 *
 *		p_running_crc - pointer to the initial or final remainder value 
 *						used in CRC computations. 
 *		p_buf - the packet buffer where crc computations are being performed
 *		length - the length of p_buf in bytes
 *		init_bytes - the number of initial bytes that need to be procesed before
 *					 aligning p_buf to multiples of 4 bytes
 *
 * Return value:
 *		
 *		The computed CRC32c value
 */

uint32_t
static crc32c_sb8_64_bit(
	uint32_t p_running_crc,
    const uint8_t*	p_buf,
    const uint32_t length,
	const uint32_t init_bytes)
{
	uint32_t li;
	uint32_t crc, term1, term2;
	uint32_t running_length;
	uint32_t end_bytes;
	
	crc = ~p_running_crc;
	running_length = ((length - init_bytes)/8)*8;
	end_bytes = length - init_bytes - running_length; 

	for(li=0; li < init_bytes; li++) 
		crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);	
	for(li=0; li < running_length/8; li++) 
	{
		crc ^= *(uint32_t *)p_buf;
		p_buf += 4;
		term1 = crc_tableil8_o88[crc & 0x000000FF] ^
				crc_tableil8_o80[(crc >> 8) & 0x000000FF];
		term2 = crc >> 16;
		crc = term1 ^
			  crc_tableil8_o72[term2 & 0x000000FF] ^ 
			  crc_tableil8_o64[(term2 >> 8) & 0x000000FF];
		term1 = crc_tableil8_o56[(*(uint32_t *)p_buf) & 0x000000FF] ^
				crc_tableil8_o48[((*(uint32_t *)p_buf) >> 8) & 0x000000FF];
		
		term2 = (*(uint32_t *)p_buf) >> 16;
		crc =	crc ^ 
				term1 ^		
				crc_tableil8_o40[term2  & 0x000000FF] ^	
				crc_tableil8_o32[(term2 >> 8) & 0x000000FF];	
		p_buf += 4;
	}
	for(li=0; li < end_bytes; li++) 
		crc = crc_tableil8_o32[(crc ^ *p_buf++) & 0x000000FF] ^ (crc >> 8);
	
    return ~crc;
}

uint32 memcrc32(const void* inData, uint32 inLength, uint32 inCrc)
{
	const uint8* __restrict data = reinterpret_cast<const uint8_t*>(inData);

	// we need to align to 32-bits
	const uint32 initBytes = (const uint8*)(((std::uintptr_t)data + 3) & ~3) - data;

	return crc32c_sb8_64_bit(inCrc, data, inLength, initBytes);
}

uint32 strihash(const char* inData)
{
	uint32 result = 0;
	while (*inData)
	{
		const uint8 c = (uint8)::toupper(*inData++);
		result = crc_tableil8_o32[(result ^ c) & 0x000000FF] ^ ((result >> 8) & 0x00FFFFFF);
	}
	return result;
}

uint32 strcrc32(const char* inData, uint32 inCrc)
{
	inCrc = ~inCrc;
	while (char c = *inData++)
	{
		inCrc = (inCrc >> 8) ^ crc_tableil8_o32[(inCrc ^ c) & 0x000000FF];
		inCrc = (inCrc >> 8) ^ crc_tableil8_o32[(inCrc    ) & 0x000000FF];
		inCrc = (inCrc >> 8) ^ crc_tableil8_o32[(inCrc    ) & 0x000000FF];
		inCrc = (inCrc >> 8) ^ crc_tableil8_o32[(inCrc    ) & 0x000000FF];
	}
	return ~inCrc;
}

NS_END
