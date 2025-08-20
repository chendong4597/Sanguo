/*!
 * \file SubFileSystem.cpp
 *
 * \author zjhlogo
 * \date 2019/07/08
 *
 * 
 */
#include "SubFileSystem.h"
#include "File.h"
#include "../LogUtil.h"

NS_BEGIN

SubFileSystem::PathRedirectCallback SubFileSystem::m_defaultRedirectionCb;


SubFileSystem::SubFileSystem(int prop, int storgeSpace, const PathRedirectCallback& pathRedirectCb)
    :m_property(prop)
    , m_storageSpace(storgeSpace)
    , m_pathRedirectCb(pathRedirectCb)
{

}

SubFileSystem::~SubFileSystem()
{

}

tstring SubFileSystem::redirectPath(const tstring& filePath)
{
    if (m_pathRedirectCb)
    {
        return m_pathRedirectCb(filePath);
    }
    
    return filePath;
}

bool SubFileSystem::readFileIntoBuffer(BufferData& bufferOut, const tstring& filePath, int flag)
{
    auto file = openFile(filePath, File::AM_READ);
    if (!file) return false;

    if (file->read(bufferOut, file->getFileSize()) != file->getFileSize())
    {
        LOGE("Read file failed, {0}\n", filePath);
        return false;
    }

    return true;
}

void SubFileSystem::setDefaultRedirectionCb(const PathRedirectCallback& cb)
{
    m_defaultRedirectionCb = cb;
}

const SubFileSystem::PathRedirectCallback& SubFileSystem::getDefaultRedirectionCb()
{
    return m_defaultRedirectionCb;
}

NS_END
