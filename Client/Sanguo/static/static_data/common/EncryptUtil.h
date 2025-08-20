#pragma once

#include <baseutil.h>
#include <functional>

class EncryptUtil
{
public:
	using FilePathRedirectCallback = std::function<bzbee::tstring(const bzbee::tstring&)>;

	static bzbee::tstring EncryptString(const char* encoded, const char* ignore, bool useHexEncode = false);
	static bzbee::tstring EncryptString(const char* encoded, bool useHexEncode = false);
	static void HexEncrypt(bzbee::tstring &cipher, bzbee::tstring &encoded);
	static bzbee::tstring DecodeString(const bzbee::tstring & encoded, bool useHexEncode = false);
	static void HexDecode(const bzbee::tstring & encoded, bzbee::tstring &cipher);
	static bool HexDecodeString(bzbee::BufferData& bufferOut, const char*hex);
	static bzbee::tstring EncrptBySHA(bzbee::tstring data);

    static bzbee::tstring getEncryptedFilePath(const bzbee::tstring& filePath);
	static void setFilePathRedirectCallback(const FilePathRedirectCallback& cb) {
		m_filePathRedirectCallback = cb;
	}

    // this is the adapter for XmlUtil::DataEncryptFunc call back
    static bool EncodeData(bzbee::tstring& dataOut, const bzbee::tstring& dataIn, const bzbee::tstring& filePath);
    static bool DecodeData(bzbee::tstring& dataOut, const bzbee::tstring& dataIn, const bzbee::tstring& filePath);

private:
    static const bzbee::tstring ENCRYPT_KEY;
    static const bzbee::tstring ENCRYPT_IV;

	static FilePathRedirectCallback m_filePathRedirectCallback;
};
