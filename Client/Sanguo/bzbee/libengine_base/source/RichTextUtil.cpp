/*!
* \file RichTextUtil.cpp
*
* \author ChenBK
* \date 06/13/2016
*
*
*/
#include "RichTextUtil.h"

NS_BEGIN

//拆分 XmlFont 文本
bool RichTextUtil::format(tstring& outText, const tstring& text)
{
	int currentLayer = 1;
	tstring htmlText = EMPTY_STRING;
	tstring::const_iterator pIndex = text.begin();
	tstring::const_iterator pEndIndex = text.end();
	tstring defaultHtmlTag = "<font>";
	tstring defaultHtmlCloseTag = "</font>";

	TV_HTML_TEXT outHtmlTextList;
	TS_HTML_TAG  tagStack;

	//添加默认最外层
	tagStack.push(HTML_TAG(currentLayer, defaultHtmlTag));

	// 开始处理
	while (pIndex != pEndIndex)
	{
		//处理查找 < 符号
		if (*pIndex == '<')
		{
			// 处理CloseTag 类示 </font> 适用
			if ((pIndex + 1) != pEndIndex && *(pIndex + 1) == '/')
			{
				if (!popCloseTag(pIndex, pEndIndex, currentLayer,htmlText,outHtmlTextList,tagStack))
				{
					return false;
				}
				continue;
			}

			pushNewTag(pIndex, pEndIndex, currentLayer, htmlText, outHtmlTextList, tagStack);

			// <font><font> 适用
			if (pIndex == pEndIndex || *pIndex == '<')
			{
				continue;
			}
		}

		htmlText += *pIndex++;
	}// End while(m_pIndex != pEndIndex)

	//最终再检查一下外层是否还有未 Close 的 Tag
	if (htmlText.size() > 0)
	{
		outHtmlTextList.push_back(HTML_TEXT(currentLayer, tagStack.top().m_tag + htmlText));
	}

	for (HTML_TEXT& x : outHtmlTextList)
	{
		if (x.m_tagLayer == currentLayer && *(x.m_htmlText.end() - 1) != '>')
		{
			x.m_htmlText += defaultHtmlCloseTag;
		}
	}

	//判断最终只剩一个 默认Tag, 如果不是,则表示 Tag 未全部 Close
	if (tagStack.size() == 1)
	{
		tagStack.pop();
	}
	else
	{
		return false;
	}

	////输出解释结果
	outText += "<E>";
	for (const auto& item : outHtmlTextList)
	{
		outText += item.m_htmlText;
	}
	outText += "</E>";
	return true;
}


//查找CloseTag 类似 </font>
bool RichTextUtil::popCloseTag(tstring::const_iterator& pIndex, tstring::const_iterator& pEndIndex, int& currentLayer, tstring& htmlText, TV_HTML_TEXT& outHtmlTextList, TS_HTML_TAG& tagStack)
{
	tstring closeTag;
	while (pIndex != pEndIndex && *pIndex != '>')
	{
		closeTag += *pIndex++;
	}
	if (pIndex != pEndIndex)
	{
		pIndex++;
	}
	closeTag += '>';

	if (htmlText.size() > 0)
	{
		outHtmlTextList.push_back(HTML_TEXT(currentLayer, tagStack.top().m_tag + htmlText));
	}

	//删除一个CloseTag时, 把前面同层级的补上给补全.
	for (HTML_TEXT& x : outHtmlTextList)
	{
		if (x.m_tagLayer == currentLayer && *(x.m_htmlText.end() - 1) != '>')
		{
			x.m_htmlText += closeTag;
		}
	}
	currentLayer--;
	htmlText.clear();

	// TagStack 不可能小1, 小于说明输入的Tag层级不正确.
	if (tagStack.size() > 0)
	{
		tagStack.pop();
	}
	else
	{
		return false;
	}

	return true;
}

//查找NewTag 类似 <font> 或 <img src="" />
void RichTextUtil::pushNewTag(tstring::const_iterator& pIndex, tstring::const_iterator& pEndIndex, int& currentLayer, tstring& htmlText, TV_HTML_TEXT& outHtmlTextList, TS_HTML_TAG& tagStack)
{
	tstring newTag;
	while (pIndex != pEndIndex && *pIndex != '>')
	{
		newTag += *pIndex++;
	}
	if (pIndex != pEndIndex)
	{
		pIndex++;
	}

	newTag += '>';

	// <img xxxx /> 适用 , else <font>xxxx</font> 适用
	if (*(newTag.end() - 2) == '/')
	{
		if (htmlText.size() > 0)
		{
			outHtmlTextList.push_back(HTML_TEXT(currentLayer, tagStack.top().m_tag + htmlText));
			htmlText.clear();
		}

		outHtmlTextList.push_back(HTML_TEXT(currentLayer, newTag));
	}
	else
	{
		if (htmlText.size() > 0)
		{
			outHtmlTextList.push_back(HTML_TEXT(currentLayer, tagStack.top().m_tag + htmlText));
			htmlText.clear();
		}

		tagStack.push(HTML_TAG(++currentLayer, newTag));
	}
}

// 清除 Html Tag
void RichTextUtil::clearHtml(tstring& outText, const tstring& inHtml)
{
	bool bIgnore{ false };
	tstring::const_iterator pIndex = inHtml.begin();
	tstring::const_iterator pEndIndex = inHtml.end();

	// 开始处理
	while (pIndex != pEndIndex)
	{
		//处理查找 < 符号
		if (*pIndex == '<')
		{
			bIgnore = true;
		}
		// 添加到内容
		if(!bIgnore)
		{
			outText += *pIndex;
		}
		//处理查找 > 符号
		if (*pIndex == '>')
		{
			bIgnore = false;
		}

		*pIndex++;
	}
}

tstring RichTextUtil::formatToXml(const tstring& strText, const tstring& defaultTag)
{
	if (strText == EMPTY_STRING) return EMPTY_STRING;
	const tstring tagBeg = "<" + defaultTag + ">";
	const tstring tagEnd = "</" + defaultTag + ">";
	tstring::size_type allTagLength = tagBeg.length() + tagEnd.length();

	tstring::size_type ltIdx = strText.find('<');
	if (ltIdx != tstring::npos)
	{
		tstring strCopy = strText;
		if (ltIdx > 0)
		{
			strCopy.insert(ltIdx, tagEnd);
			strCopy.insert(0, tagBeg);
		}
		tstring::size_type rtIdx = strCopy.rfind('>');
		if (rtIdx != tstring::npos && strCopy.size() > rtIdx + 1)
		{
			strCopy.insert(rtIdx + 1, tagBeg);
			strCopy.append(tagEnd);
		}

		rtIdx = strCopy.find('>');
//		tstring::size_type tagOffset = 0;
		while (rtIdx != tstring::npos && rtIdx < strCopy.size() - 1)
		{
			tstring::size_type tagOffset = 0;
			ltIdx = strCopy.find('<', rtIdx);
			if (ltIdx != tstring::npos && ltIdx - rtIdx > 1 && strCopy[ltIdx + 1] != '/')
			{
				strCopy.insert(ltIdx, tagEnd);
				strCopy.insert(rtIdx + 1, tagBeg);
				tagOffset = allTagLength;
			}
			rtIdx = strCopy.find('>', ltIdx + tagOffset);
		}

		return strCopy;
	}
	else
	{
		return tagBeg + strText + tagEnd;
	}
}

NS_END
