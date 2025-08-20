/*!
 * \file FileSystem.h
 *
 * \author zjhlogo
 * \date 2019/06/20
 *
 * 
 */
#pragma once
#include "File.h"
#include "SubFileSystem.h"
#include <vector>

NS_BEGIN

class FileSystem
{
public:
    typedef std::vector<SubFileSystem*> SubFileSystemList;

public:
    //! check file exist by path
    /* 
     * \param filePath the file path
     * \param storage check file in specific storage space, default 0 means no specific storage require.
     * \return bool return true if exist, otherwise return false
     */
    static bool isFileExist(const tstring& filePath, int storage = 0);

	//! get file location in a storage space
	/* 
	 * \param filePath would be the absolute path after success locate the file
	 * \param offset the offset of the file in zip package, for non-zip file system, always 0
	 * \param length the size of the file in zip package, for non-zip file system, always 0
	 * \return bool return true if found the file, otherwise return false
	 */
	static bool getFileLocation(tstring& filePath, long& offset, long& length);

    //! open a file for read only
    /* 
     * \param filePath the file path
     * \param storage search file in specific storage space, default 0 means no specific storage require.
     * \return std::shared_ptr<File> return the share pointer of the file instance, will be auto close the file handle after destruction
     */
    static std::shared_ptr<File> openFile(const tstring& filePath, int storage = 0);

    //! read file into a buffer object
    /* 
     * \param bufferOut the buffer object to store the file data
     * \param filePath the file path
     * \param storage search file in specific storage space, default 0 means no specific storage require.
     * \return bool return true if read success, otherwise return false
     */
    static bool readFileIntoBuffer(BufferData& bufferOut, const tstring& filePath, int storage = 0);

    //! create a file for read/write, will auto create sub-folder when nessary
    /* 
     * \param filePath the file path
     * \param storage create file in specific storage space
     * \param mode create file mode
     * \return std::shared_ptr<File> return the share pointer of the file instance, will be auto close the file handle after destruction
     */
    static std::shared_ptr<File> createFile(const tstring& filePath, int storage, int mode = File::AM_WRITE);

    //! delete a file in specific storage space
    /* 
     * \param filePath the file path
     * \param storage the storage space of the file
     * \return bool return true if delete success, otherwise return false
     */
    static bool deleteFile(const tstring& filePath, int storage);

    //! add a sub file system
    /* 
     * \param subFileSystem the file system to add
     * \param priority the file system priority, default -1 the new sub-file system will be first search then others priority with -1
     * \return bool return true if add success, otherwise return false
     */
    static bool addSubFileSystem(SubFileSystem* subFileSystem, int priority = -1);
    static bool removeSubFileSystem(SubFileSystem* subFileSystem);

    //! clear all sub-file systems
    /* 
     * \return void
     */
    static void cleanup();

    //! find sub-file system with priority
    /* 
     * \param priority
     * \return T* return the sub-file system found
     */
    template <typename T> static T* findSubFileSystem(int priority = -1)
    {
        for (SubFileSystemList::reverse_iterator it = m_subFileSystems.rbegin(); it != m_subFileSystems.rend(); ++it)
        {
            SubFileSystem* subFileSystem = *it;

            auto found = dynamic_cast<T*>(subFileSystem);
            if (!found) continue;

            if (priority == -1 || priority == subFileSystem->getPriority())
            {
                return found;
            }
        }

        return nullptr;
    }

private:
    static SubFileSystemList m_subFileSystems;

};

NS_END
