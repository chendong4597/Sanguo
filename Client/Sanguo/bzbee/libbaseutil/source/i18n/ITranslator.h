
/*!
* \file ITranslator.h
* \date 05-24-2014 15:22:00
*
*
* \author cs 04nycs@gmail.com
*/
#pragma once

#include "../BaseType.h"
#include <map>

NS_BEGIN

class ITranslator 
{
public:
	virtual const tstring translateText(const tstring& key) const = 0;
	virtual const tstring translateFontId(const tstring& strOldFont) const = 0;
};

class Translator : public ITranslator
{
public:
	enum CONST_DEFINE : unsigned
	{
		MSGCATALOG_MAGIC = 0x950412DE,
	};

	// header of a .mo file
	typedef struct MSG_CATALOG_HEADER_tag
	{
		uint magic;          // offset +00:  magic id
		uint revision;       //        +04:  revision
		uint numStrings;     //        +08:  number of strings in the file
		uint ofsOrigTable;   //        +0C:  start of original string table
		uint ofsTransTable;  //        +10:  start of translated string table
		uint nHashSize;      //        +14:  hash table size
		uint ofsHashTable;   //        +18:  offset of hash table start
	} MSG_CATALOG_HEADER;

	// an entry in the string table
	typedef struct MSG_TABLE_ENTRY_tag
	{
		uint nLen;           // length of the string
		uint ofsString;      // pointer to the string
	} MSG_TABLE_ENTRY;

	typedef std::map<tstring, tstring> TM_STRING;

public:
	Translator(const tstring& strLan = EMPTY_STRING);
	virtual ~Translator();

	bool initialize(const tstring& strFile);
	virtual const tstring translateText(const tstring& key) const override;
	virtual const tstring translateFontId(const tstring& strOldFontId) const override;

private:
	bool parseMsgCatalog(const uchar* pBuffer);

private:
	TM_STRING m_TransMap;
	tstring m_strLan;
};

class TranslatorsMgr
{
public:
	typedef std::map<tstring, Translator*> TM_TRANSLATOR;

public:
	static TranslatorsMgr& getInstance();

public:
	TranslatorsMgr();
	virtual ~TranslatorsMgr();

	const Translator* getTranslator(const tstring& strLan);

private:
	TM_TRANSLATOR m_mTranslators;
};

NS_END
