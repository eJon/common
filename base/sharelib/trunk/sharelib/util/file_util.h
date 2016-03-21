#ifndef SHARELIB_UTIL_FILE_UTIL_H
#define SHARELIB_UTIL_FILE_UTIL_H

#include <sharelib/common.h>
#include <tr1/memory>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sharelib/util/string_util.h>
#include <map>
SHARELIB_BS;

class FileUtil
{
public:
    FileUtil();
    ~FileUtil();
public:
    static int GetFileStatus(const std::string &filePath, struct stat *buf);
public:
    static std::string ReadLocalFile(const std::string &filePath);
    static bool ReadLocalFile(const std::string &filePath, std::vector<std::string>& content);


    static std::string ReadLocalFileOriginalData(const std::string &filePath);

    static bool WriteLocalFile(const std::string &filePath,
                               const std::string &content);

    static int WriteLocalFileByDate(const std::string &filePath,
                                     const std::string &content);

    static bool DeleteLocalFile(const std::string &localFilePath);

    static bool CompFile(const std::string& first, const std::string& second);

    static int CompFile(const char* first, const char* second);

    static bool IsFileExist(const std::string& file);
    static bool IsDirExist(const std::string& dir);
    
    static bool MakeLocalDir(const std::string &localDirPath, 
                             bool recursive = false);
    static bool RecursiveMakeLocalDir(const std::string &localDirPath);

    static bool RemoveLocalDir(const std::string &localDirPath,
                               bool removeNoneEmptyDir = false);
    static bool RecursiveRemoveLocalDir(const std::string &localDirPath);
    
    static std::string GenerateAbsolutePath(const std::string &directory, 
                  const std::string &file_path);
public:
    static bool CopyLocalFile(const std::string &desFilePath,
                              const std::string &srcFilePath);
    //0:all 1:file 2:dir
    static bool ListLocalDir(const std::string& dirName, 
                             std::vector<std::string>& entryVec,
                             int flag = 0);//bool fileOnly = true);
        
    static std::string GetAbsFile(std::string parent, std::string fileName);
public:
    static std::string GetParentDir(const std::string &currentDir);
public:
    static bool Parse(std::string& file, std::map<std::string,std::string>& map);
public:
    static bool Rename(std::string oldfile, std::string newfile);
private:
    static const char DIR_DELIM = '/';
};

typedef std::tr1::shared_ptr<FileUtil> FileUtilPtr;

SHARELIB_ES;

#endif //SHARELIB_FILE_UTIL_H
