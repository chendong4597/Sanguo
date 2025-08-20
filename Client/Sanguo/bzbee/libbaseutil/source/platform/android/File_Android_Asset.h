/*!
 * \file File_Android_Asset.h
 *
 * \author zjhlogo
 * \date 2019/06/24
 *
 * 
 */
#pragma once
#include "../../file/File.h"
#include <android/asset_manager_jni.h>

NS_BEGIN

class File_Android_Asset : public File
{
public:
    File_Android_Asset(AAssetManager* assetManager);
    virtual ~File_Android_Asset();

    bool open(const tstring& filePath);
    virtual void close() override;

    virtual I64 seek(I64 offset, SeekType seekType = SeekType::Current) override;
    virtual I64 tell() override;

    virtual I64 read(void* dataOut, I64 size) override;
    virtual I64 read(BufferData& dataOut, I64 size) override;

    virtual I64 write(const void* data, I64 size) override;
    virtual I64 write(const BufferData& data) override;

private:
    AAssetManager* m_assetManager{};
    AAsset* m_asset{};

};

NS_END
