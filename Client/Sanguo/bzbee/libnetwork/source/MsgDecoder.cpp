/*!
 * \file IMsgDecoder.h
 *
 * \author zhijieli
 * \date July 2016
 *
 *
 */
#include "MsgDecoder.h"
#include <utils/BufferStream.h>
#include <LogUtil.h>
#include <algorithm>
#include <md5.h>

NS_BEGIN

bool MsgDecoder::decodeMsg(Msg*& pMsgOut, tstring& strMd5Out, BufferStreamWriter& bufferInOut, uint16& nMsgId)
{
	pMsgOut = nullptr;
	// decode the message
	BufferStreamReader bufferReader(bufferInOut.getBuffData());

	// read message block size
	uint16 nMsgSize = 0;
	if (!bufferReader.read(&nMsgSize, sizeof(nMsgSize))) return false;
	if (bufferReader.getSize() + sizeof(nMsgSize) < nMsgSize)
		return false;

	const uint16 msgHeadLength = sizeof(uint16) + sizeof(uint16);
	// check message valid
	if (nMsgSize < msgHeadLength) return false;	// all message start with: msgSize(uint16) + msgId(uint16)

	// read message id
	if (!bufferReader.read(&nMsgId, sizeof(nMsgId))) return false;

	pMsgOut = ProtobufMsgMgr::getInstance().createMessage(nMsgId);
	if (pMsgOut)
	{
		if (!pMsgOut->ParseFromArray(bufferReader.getData(), nMsgSize - msgHeadLength))
		{
			LOGE("parse message failed, type={0}, id={1}", pMsgOut->GetTypeName(), nMsgId);
			SAFE_DELETE(pMsgOut);
		}
		else
		{
			// dispatch message
// 			LOGD("receive message: type={0}, size={1}", pMsgOut->GetTypeName(), nMsgSize);
		}
	}
	else
	{
		LOGE("unknown message msgId={0}, size={1}", nMsgId, nMsgSize);
	}

	// get md5 string
	bufferReader.reset();
	MD5 md5;
	md5.update((const char*)bufferReader.getData(), nMsgSize);
	md5.finalize();
	strMd5Out = md5.hexdigest();

	// point to next message
	bufferReader.skip(nMsgSize);

	// adjust output buffer
	int remainSize = bufferReader.getSize();
	if (remainSize > 0)
	{
		// copy remain buffer into temp buffer
		if ((int)m_bufferTemp.size() < remainSize) m_bufferTemp.resize(remainSize);
		std::copy(bufferReader.getData(), bufferReader.getData() + remainSize, m_bufferTemp.begin());

		// copy temp buffer into bufferInOut
		bufferInOut.reset();
		bufferInOut.write(&m_bufferTemp[0], remainSize);
	}
	else
	{
		bufferInOut.reset();
	}

	return (pMsgOut || bufferInOut.getSize() > 0);
}


ProtobufMsgMgr& ProtobufMsgMgr::getInstance()
{
	static ProtobufMsgMgr s_protobufMsgMgr;
	return s_protobufMsgMgr;
}

ProtobufMsgMgr::ProtobufMsgMgr()
{

}

ProtobufMsgMgr::~ProtobufMsgMgr()
{
	m_FuncCreatorMap.clear();
}

bool ProtobufMsgMgr::registerMsgCreator(uint16 nMsgId, const FuncMsgCreator& funcCreator)
{
	const auto& pair = m_FuncCreatorMap.find(nMsgId);
	if (pair == m_FuncCreatorMap.end())
	{
		CreatorInfo info;
		info.creator = funcCreator;
		info.m_reference = 1;
		m_FuncCreatorMap.insert(std::make_pair(nMsgId, info));
	}
	else
	{
		pair->second.m_reference++;
	}

	return true;
}

bool ProtobufMsgMgr::unregisterMsgCreator(uint16 nMsgId)
{
	const auto& pair = m_FuncCreatorMap.find(nMsgId);
	if (pair != m_FuncCreatorMap.end())
	{
		pair->second.m_reference--;
		if (pair->second.m_reference <= 0)
		{
			m_FuncCreatorMap.erase(pair);
		}
	}

	return true;
}

Msg* ProtobufMsgMgr::createMessage(uint16 nMsgId)
{
	const auto& pair = m_FuncCreatorMap.find(nMsgId);
	if (pair != m_FuncCreatorMap.end())
	{
		return pair->second.creator();
	}

	return nullptr;
}

NS_END
