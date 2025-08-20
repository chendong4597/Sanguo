/*!
 * \file RichTextUtil.h
 *
 * \author ChenBK
 * \date 06/13/2016
 *
 *
 */
#pragma once
#include "BaseType.h"
#include <FileUtil.h>
#include <stack>

NS_BEGIN

class RichTextUtil
{
public:
	typedef struct HTML_TEXT_tag
	{
	public:
		HTML_TEXT_tag(int tagLayer, const tstring& htmlText)
		{
			m_tagLayer = tagLayer;
			m_htmlText = htmlText;
		}

		int m_tagLayer{ 0 };
		tstring m_htmlText;
	} HTML_TEXT;

	typedef struct HTML_TAG_tag
	{
	public:
		HTML_TAG_tag(int tagLayer, const tstring& tag)
		{
			m_tagLayer = tagLayer;
			m_tag = tag;
		}

		int m_tagLayer{};
		tstring m_tag;
	} HTML_TAG;

	typedef std::vector<HTML_TEXT> TV_HTML_TEXT;
	typedef std::stack<HTML_TAG> TS_HTML_TAG;

public:
	static bool format(tstring& outText, const tstring& text);
	static void clearHtml(tstring& outText, const tstring& inHtml);
	static tstring formatToXml(const tstring& strText, const tstring& defaultTag);

private:
	static bool popCloseTag(tstring::const_iterator& pIndex, tstring::const_iterator& pEndIndex,int& currentLayer, tstring& htmlText, TV_HTML_TEXT& outHtmlTextList, TS_HTML_TAG& tagStack);
	static void pushNewTag(tstring::const_iterator& pIndex, tstring::const_iterator& pEndIndex, int& currentLayer, tstring& htmlText, TV_HTML_TEXT& outHtmlTextList, TS_HTML_TAG& tagStack);

};

NS_END
