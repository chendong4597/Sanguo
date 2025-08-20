/*!
 * \file Cache.h
 *
 * \author laizhisheng
 * \date 2016/12/02 13:33
 *
 */
#pragma once
#include "BaseType.h"
#include "tinyxml2.h"

NS_BEGIN

class Cache
{
public:
	static Cache& getInstance();

	bool initialize();
	void terminate();

	bool SetIPAndIggId(tstring strIP, I64 iggId);

	int getInt(const tstring& key, int defaultValue);
	void setInt(const tstring& key, int value);

	I64 getInt64(const tstring& key, I64 defaultValue);
	void setInt64(const tstring& key, I64 value);

	tstring getString(const tstring& key, const tstring& defaultValue);
	void setString(const tstring& key, const tstring& value);

	bool getData(const tstring& key, BufferData& dataOut);
	bool setData(const tstring& key, const BufferData& data);

	struct LoginServerAddress
	{
		tstring ip;
		int port;
	};
	void setLoginServers(std::vector<LoginServerAddress>& vec);
	LoginServerAddress chooseLoginServer(void) const;

	void flush();
	void removeCache();

	inline tstring getLoginServer() const { return m_loginServer; }
	inline I64 getIggId() const { return m_iggId; }

	I64 getGlobalInt64(const tstring& key, I64 defaultValue);
	void setGlobalInt64(const tstring& key, I64 value);
	tstring getGlobalString(const tstring& key, const tstring& defaultValue);
	void setGlobalString(const tstring& key, const tstring& value);
	void flushGlobal();

	const tstring& getUserBehaviourLogPath();

private:
	tinyxml2::XMLDocument m_xmlDoc;
	tinyxml2::XMLDocument m_xmlGlobalDoc;

	std::vector<LoginServerAddress> m_loginServerArray;
	tstring m_filePath;
	tstring m_loginServer;
	I64 m_iggId{};
};

NS_END