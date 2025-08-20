/*!
 * \file SubFileSystem.h
 *
 * \author zjhlogo
 * \date 2019/06/20
 *
 * 
 */
#pragma once
#include "File.h"
#include <memory>
#include <functional>

NS_BEGIN

class SubFileSystem
{
public:
    enum FILE_PROPERTY
    {
        FP_READABLE = 1 << 0,
        FP_WRITABLE = 1 << 1,

        FP_READ_WRITE = FP_READABLE | FP_WRITABLE,
    };

    enum FILE_STORAGE
    {
        FS_ASSET    = 1 << 0,
        FS_INTERNAL = 1 << 1,
        FS_EXTERNAL = 1 << 2,

        FS_ALL      = FS_ASSET | FS_INTERNAL | FS_EXTERNAL,
    };

    typedef std::function<tstring(const tstring&)> PathRedirectCallback;

public:
    SubFileSystem(int prop, int storgeSpace, const PathRedirectCallback& pathRedirectCb);
    virtual ~SubFileSystem();

    bool checkProperty(int prop) { return (m_property & prop) == prop; };
    bool acceptStorage(int storageSpace) { return (storageSpace & m_storageSpace) == storageSpace; };

    void setPriority(int priority) { m_priority = priority; };
    int getPriority() const { return m_priority; };

    tstring redirectPath(const tstring& filePath);

    virtual bool getFileLocation(tstring& filePath, long& offset, long& length) = 0;
    virtual bool searchFile(const tstring& filePath) = 0;
    virtual std::shared_ptr<File> openFile(const tstring& filePath, int flag) = 0;
    virtual bool deleteFile(const tstring& filePath) = 0;

    virtual bool readFileIntoBuffer(BufferData& bufferOut, const tstring& filePath, int flag);

    //! set default file redirection callback function, this is used for redirect a path to another path by using the callback function
    /* 
     * \param cb the callback function return the new path for a given path
     * \return void
     */
    static void setDefaultRedirectionCb(const PathRedirectCallback& cb);

    //! get default redirection callback
    /* 
     * \return const PathRedirectCallback&
     */
    static const PathRedirectCallback& getDefaultRedirectionCb();

protected:
    int m_property{};
    int m_storageSpace{};
    int m_priority{};
    PathRedirectCallback m_pathRedirectCb;

    static PathRedirectCallback m_defaultRedirectionCb;

};

NS_END
