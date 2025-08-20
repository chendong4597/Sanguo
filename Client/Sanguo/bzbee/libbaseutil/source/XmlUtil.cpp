/*!
 * \file XmlUtil.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 05/13/2016
 *
 *
 */
#include "XmlUtil.h"
#include "file/File.h"
#include "file/FileSystem.h"
#include "FileUtil.h"
#include "LogUtil.h"

NS_BEGIN

XmlUtil::DataEncryptFunc XmlUtil::m_encoder;
XmlUtil::DataEncryptFunc XmlUtil::m_decoder;

bool XmlUtil::loadXmlFile(tinyxml2::XMLDocument& doc, const tstring& filePath, int storage /* = 0 */)
{

	tstring strXmlData;
    if (!FileUtil::readFileIntoString(strXmlData, filePath, storage))
    {
        LOGE(("load file failed, {0}"), filePath);
        return false;
    }

    if (m_decoder)
    {
        m_decoder(strXmlData, strXmlData, filePath);
    }

    if (doc.Parse(strXmlData.c_str()) != tinyxml2::XML_SUCCESS)
	{
		LOGE("parse xml file failed, file={0}, error={1}", filePath, doc.ErrorName());
        if (doc.GetErrorStr1()) LOGE("Error String1: {0}", doc.GetErrorStr1());
        if (doc.GetErrorStr2()) LOGE("Error String2: {0}", doc.GetErrorStr2());

        return false;
	}

	return true;
}

std::future<tinyxml2::XMLDocument*> XmlUtil::asyncLoadXmlFile(const tstring& filePath, int storage /* = 0 */)
{
	return std::async(std::launch::async, [filePath, storage] { tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument; loadXmlFile(*doc, filePath, storage); return doc; });
}

bool XmlUtil::saveXmlFile(tinyxml2::XMLDocument& doc, const tstring& filePath, int storage /* = SubFileSystem::FS_EXTERNAL */, bool encode /* = false */)
{
	tinyxml2::XMLPrinter printer;
	doc.Print(&printer);
	tstring tempDoc = printer.CStr();

    if (encode && m_encoder)
    {
        m_encoder(tempDoc, tempDoc, filePath);
    }

    auto file = FileSystem::createFile(filePath, storage);
    if (!file) return false;

	file->write(tempDoc.c_str(), tempDoc.length()-1);
	file->close();

	return true;
}

NS_END
