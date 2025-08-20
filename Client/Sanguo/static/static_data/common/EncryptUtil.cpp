#include "EncryptUtil.h"
#include <FileUtil.h>
#include <StringUtil.h>
#include <iostream>

#include "cryptopp/aes.h"
using CryptoPP::AES;

#include "cryptopp/ccm.h"
using CryptoPP::CTR_Mode;

#include "cryptopp/cryptlib.h"
using CryptoPP::Exception;
using CryptoPP::SecByteBlock;

#include "cryptopp/hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "cryptopp/filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include "cryptopp/sha.h"

const bzbee::tstring EncryptUtil::ENCRYPT_KEY = "AC5E19B23533F48A75CDE4C3E81AF45C";
const bzbee::tstring EncryptUtil::ENCRYPT_IV = "D01575950E9F43EF6E04C8A3AEC6BCAA";
EncryptUtil::FilePathRedirectCallback EncryptUtil::m_filePathRedirectCallback;

bool EncryptUtil::HexDecodeString(bzbee::BufferData& bufferOut, const char*hex)
{
	StringSource ss(hex, true, new HexDecoder);
    bufferOut.resize(static_cast<size_t>(ss.MaxRetrievable()));
	ss.Get(bufferOut.data(), bufferOut.size());
    return true;
}

bzbee::tstring EncryptUtil::EncryptString(const char* original, bool usedHexCode)
{
	bzbee::tstring cipher, encoded;
	try
	{
		//cout << "plain text: " << plain << endl;
		CryptoPP::byte* byte_key = (CryptoPP::byte*)ENCRYPT_KEY.c_str();
		CryptoPP::byte* byte_iv = (CryptoPP::byte*)ENCRYPT_IV.c_str();

		CTR_Mode< AES >::Encryption e(byte_key, ENCRYPT_KEY.length(), byte_iv);


		// The StreamTransformationFilter adds padding
		//  as required. ECB and CBC Mode must be padded
		//  to the block size of the cipher.
		StringSource(original, true,
			new StreamTransformationFilter(e,
				new StringSink(cipher)
			) // StreamTransformationFilter      
		); // StringSource
		encoded = cipher;
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	if (usedHexCode)
	{
		encoded = "";
		HexEncrypt(cipher, encoded);
	}
	//cout << "encoded text: " << encoded << endl;

	return encoded;
}

void EncryptUtil::HexEncrypt(bzbee::tstring &cipher, bzbee::tstring &encoded)
{
	// Pretty print
	StringSource(cipher, true,
		new HexEncoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
}

bzbee::tstring EncryptUtil::DecodeString(const bzbee::tstring & encoded, bool usedHexCode)
{
	bzbee::tstring  recovered, cipher;
	try
	{
		//cout << "encoded text: " << encoded << endl;
		CryptoPP::byte* byte_key = (CryptoPP::byte*)ENCRYPT_KEY.c_str();
		CryptoPP::byte* byte_iv = (CryptoPP::byte*)ENCRYPT_IV.c_str();

		CTR_Mode< AES >::Encryption d(byte_key, ENCRYPT_KEY.length(), byte_iv);


		// The StreamTransformationFilter removes
		if (usedHexCode)
		{
			cipher = "";
			HexDecode(encoded, cipher);
		}
		StringSource(encoded, true,
			new StreamTransformationFilter(d,
				new StringSink(recovered)
			) // StreamTransformationFilter      
		); // StringSource

		//cout << "recovered text: " << recovered << endl;
	}
	catch (const CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}
	return recovered;
}

void EncryptUtil::HexDecode(const bzbee::tstring & encoded, bzbee::tstring &cipher)
{
	//  padding as required.
	StringSource(encoded, true,
		new HexDecoder(
			new StringSink(cipher)
		) // HexEncoder
	); // StringSource
}

bzbee::tstring EncryptUtil::EncryptString(const char * encoded, const char * ignore, bool useHexEncode)
{
	std::vector<bzbee::tstring> arrValue;
	bzbee::StringUtil::splitString(arrValue, encoded, ignore);
	if (arrValue.size() < 2)
	{
		return EncryptString(encoded, useHexEncode);
	}
	bzbee::tstring result = EncryptString(arrValue[0].c_str(), useHexEncode);

	for (size_t i = 1; i < arrValue.size(); i++)
	{
		result += ignore;
		result += EncryptString(arrValue[i].c_str(), useHexEncode);
	}
	return result;
}

bzbee::tstring EncryptUtil::EncrptBySHA(bzbee::tstring data)
{
	CryptoPP::byte const* pbData = (CryptoPP::byte*)data.data();
	unsigned int nDataLen = data.size();
	CryptoPP::byte abDigest[CryptoPP::SHA1::DIGESTSIZE];

	CryptoPP::SHA1().CalculateDigest(abDigest, pbData, nDataLen);
	//CryptoPP::SHA3_256().VerifyDigest(abDigest, pbData, nDataLen);

	bzbee::tstring result;
	StringSource(bzbee::tstring((char*)abDigest, CryptoPP::SHA1::DIGESTSIZE), true,
		new HexEncoder(
			new StringSink(result)
		) // HexEncoder
	); // StringSource

	return result;
}

bzbee::tstring EncryptUtil::getEncryptedFilePath(const bzbee::tstring& filePath)
{
	static const std::set<bzbee::tstring> NO_COMPRESS{ "jpg", "jpeg", "png", "gif", "wav", "mp2", "mp3", "ogg", "aac", "mpg", "mpeg", "mid", "midi", "smf", "jet", "rtttl", "imy", "xmf", "mp4", "m4a", "m4v", "3gp", "3gpp", "3g2", "3gpp2", "amr", "awb", "wma", "wmv", "wem", "bnk" };

    bzbee::tstring resultPath = bzbee::FileUtil::formatFilePath(filePath);

    // redirect file path
	if (m_filePathRedirectCallback)
	{
		resultPath = m_filePathRedirectCallback(resultPath);
	}

	resultPath = EncrptBySHA(resultPath);
    bzbee::tstring strPath = resultPath.substr(0, 2);
	resultPath = strPath + "/" + resultPath.substr(2);

	auto n = filePath.find_last_of('.');
	if (n != bzbee::tstring::npos)
	{
        bzbee::tstring ext;
        bzbee::StringUtil::tolower(ext, filePath.substr(n + 1));
		if (NO_COMPRESS.find(ext) != NO_COMPRESS.end()) resultPath += ".ogg";
	}

	//LOGI("SystemUtil_Android::getEncryptedFilePath(): {0}", resultPath);
	return resultPath;
}

bool EncryptUtil::EncodeData(bzbee::tstring& dataOut, const bzbee::tstring& dataIn, const bzbee::tstring& filePath)
{
    dataOut = EncryptUtil::EncryptString(dataIn.c_str());

    return true;
}

bool EncryptUtil::DecodeData(bzbee::tstring& dataOut, const bzbee::tstring& dataIn, const bzbee::tstring& filePath)
{
    if (bzbee::FileUtil::getFileExtension(filePath) == "bin")
    {
        if (!dataIn.empty())
        {
            dataOut = EncryptUtil::DecodeString(dataIn);
        }
    }

    return true;
}
