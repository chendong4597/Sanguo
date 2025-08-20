/*!
 * \file IMsgDecoder.h
 *
 * \author zhijieli
 * \date July 2016
 *
 * 
 */

#pragma once

#include <BaseTypeDefine.h>
#include <google/protobuf/message.h>
#include <map>
#include <functional>

NS_BEGIN

typedef ::google::protobuf::Message Msg;

class BufferStreamWriter;
class MsgDecoder
{
public:
	MsgDecoder() {};
	virtual ~MsgDecoder() {};

	bool decodeMsg(Msg*& pMsgOut, tstring& strMd5Out, BufferStreamWriter& bufferInOut, uint16& nMsgId);

private:
	BufferData m_bufferTemp;
};


class ProtobufMsgMgr
{
public:
	typedef std::function<Msg*()> FuncMsgCreator;
	typedef struct CreatorInfo_tag
	{
		FuncMsgCreator creator;
		int m_reference{};
	}CreatorInfo;
	typedef std::map<uint16, CreatorInfo> FuncCreatorMap;

public:
	static ProtobufMsgMgr& getInstance();

	ProtobufMsgMgr();
	~ProtobufMsgMgr();

	bool registerMsgCreator(uint16 nMsgId, const FuncMsgCreator& funcCreator);
	bool unregisterMsgCreator(uint16 nMsgId);
	Msg* createMessage(uint16 nMsgId);

private:
	FuncCreatorMap m_FuncCreatorMap;
};

NS_END
