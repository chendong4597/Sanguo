/*!
 * \file File_Android_Asset.cpp
 *
 * \author zjhlogo
 * \date 2019/06/24
 *
 * 
 */
#include "File_Android_Asset.h"
#include "../../LogUtil.h"

NS_BEGIN

File_Android_Asset::File_Android_Asset(AAssetManager* assetManager)
    :m_assetManager(assetManager)
{

}

File_Android_Asset::~File_Android_Asset()
{
    close();
}

bool File_Android_Asset::open(const tstring& filePath)
{
    // we only accept readable access
    m_asset = AAssetManager_open(m_assetManager, filePath.c_str(), AASSET_MODE_UNKNOWN);
    if (!m_asset)
    {
        LOGE("open file failed {0}\n", filePath);
        return false;
    }

    m_mode = File::AM_READ;
    m_fileSize = AAsset_getLength(m_asset);
    m_filePath = filePath;

    return true;
}

void File_Android_Asset::close()
{
    if (m_asset)
    {
        AAsset_close(m_asset);
        m_asset = nullptr;
    }
}

I64 File_Android_Asset::seek(I64 offset, SeekType seekType)
{
    int seekFrom = SEEK_CUR;
    if (seekType == SeekType::Begin) seekFrom = SEEK_SET;
    else if (seekType == SeekType::End) seekFrom = SEEK_END;

    return AAsset_seek(m_asset, static_cast<size_t>(offset), seekFrom);
}

I64 File_Android_Asset::tell()
{
    return AAsset_seek(m_asset, 0, SEEK_CUR);
}

I64 File_Android_Asset::read(void * dataOut, I64 size)
{
    if (size <= 0) return 0;

    return AAsset_read(m_asset, dataOut, static_cast<size_t>(size));
}

I64 File_Android_Asset::read(BufferData & dataOut, I64 size)
{
    if (size <= 0) return 0;

    dataOut.resize(static_cast<size_t>(size));
    return AAsset_read(m_asset, dataOut.data(), static_cast<size_t>(size));
}

I64 File_Android_Asset::write(const void * data, I64 size)
{
    // we don't support writing on apk
    return 0;
}

I64 File_Android_Asset::write(const BufferData & data)
{
    // we don't support writing on apk
    return 0;
}

NS_END
