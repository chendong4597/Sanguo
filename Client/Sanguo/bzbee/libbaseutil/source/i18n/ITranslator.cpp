/*!
* \file ITranslator.cpp
* \date 05-24-2014 15:22:02
*
*
* \author cs 04nycs@gmail.com
*/

#include "ITranslator.h"
#include "../FileUtil.h"
#include "../LogUtil.h"
#include "../utils/BufferStream.h"

NS_BEGIN

Translator::Translator(const tstring& strLan /*= EMPTY_STRING*/)
: m_strLan(strLan)
{

}


Translator::~Translator()
{

}

const tstring Translator::translateText(const tstring& key) const
{
	TM_STRING::const_iterator it = m_TransMap.find(key);
	if (it == m_TransMap.end())
	{
		return key;
	}

	return it->second;
}

bool Translator::initialize(const tstring& strFile)
{
	BufferData pReader;
	BufferStreamReader* pBuffer;

    FileSystem::readFileIntoBuffer(pReader, strFile);
	pBuffer = new BufferStreamReader(pReader);
	if (!pBuffer) return false;

	bool bOk = parseMsgCatalog((const uchar*)pBuffer->getData());
	SAFE_DELETE(pBuffer);

	return bOk;
}

bool Translator::parseMsgCatalog(const uchar* pBuffer)
{
	// read the header
	MSG_CATALOG_HEADER* pHeader = (MSG_CATALOG_HEADER*)(pBuffer);
	if (nullptr == pBuffer) return false;
	// examine header
	LOGI("parseMsgCatalog");
	if (pHeader->magic != MSGCATALOG_MAGIC)
	{
		LOGI("Localization::parseMsgCatalog, Invalid message catalog.");
		return false;
	}

	// initialize
	MSG_TABLE_ENTRY* pOrigTable = (MSG_TABLE_ENTRY*)(pBuffer + pHeader->ofsOrigTable);
	MSG_TABLE_ENTRY* pTransTable = (MSG_TABLE_ENTRY*)(pBuffer + pHeader->ofsTransTable);

	// 
	m_TransMap.clear();
	for (uint i = 0; i < pHeader->numStrings; ++i)
	{
		tstring key = (const tchar*)(pBuffer + pOrigTable->ofsString);
		tstring value = (const tchar*)(pBuffer + pTransTable->ofsString);

		if (!key.empty())
		{
			m_TransMap.insert(std::make_pair(key, value));
			LOGI("trans: {0} -> {1}", key, value);
		}

		++pOrigTable;
		++pTransTable;
	}

	return true;
}

const tstring Translator::translateFontId(const tstring& strOldFontId) const
{
	if (m_strLan.empty()) return strOldFontId;

	//if (FontMgr::getInstance().isCommonFont(strOldFontId))
	//{
	//	return strOldFontId;
	//}

	//return m_strLan + "/" + strOldFontId;
	return strOldFontId;
}

TranslatorsMgr& TranslatorsMgr::getInstance()
{
	static TranslatorsMgr s_instance;
	return s_instance;
}

TranslatorsMgr::TranslatorsMgr()
{

}

TranslatorsMgr::~TranslatorsMgr()
{
	for (TM_TRANSLATOR::iterator it = m_mTranslators.begin(); it != m_mTranslators.end(); ++it)
	{
		if (it->second)
		{
			delete it->second;
		}
	}

	m_mTranslators.clear();
}

const Translator* TranslatorsMgr::getTranslator(const tstring& strLan)
{
	TM_TRANSLATOR::const_iterator it = m_mTranslators.find(strLan);
	if (it == m_mTranslators.end())
	{
		Translator* pTranslator = new Translator(strLan);
		if (!pTranslator->initialize(fmt::format("config/{0}.mo", strLan)))
		{
			pTranslator = NULL;
		}

		m_mTranslators.insert(std::make_pair(strLan, pTranslator));

		return pTranslator;
	}
	else
	{
		return it->second;
	}

	return NULL;
}

NS_END