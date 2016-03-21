#include <sharelib/util/file_util.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fstream>
using namespace std;
SHARELIB_BS;
FileUtil::FileUtil(){
}

FileUtil::~FileUtil() { 
}

int FileUtil::GetFileStatus(const std::string &filePath, struct stat *buf)
{
    return stat(filePath.c_str(), buf);
}
bool FileUtil::DeleteLocalFile(const std::string &localFilePath)
{
    return (0 == unlink(localFilePath.c_str()));
}

bool FileUtil::ReadLocalFile(const std::string &filePath, std::vector<std::string>& content){
    ifstream in(filePath.c_str());
    string line;
    if (!in.is_open()) {
        return false;
    }
    while (getline(in, line)) {
        content.push_back(line);
    }
    in.close();
    return true;
}
string FileUtil::ReadLocalFile(const string &filePath) {
    ifstream in(filePath.c_str());
    stringstream ss;
    string line;
    if (!in) {
        return string("");
    }
    while (getline(in, line)) {
        ss << line;
    }
    in.close();
    return ss.str();
}

string FileUtil::ReadLocalFileOriginalData(const string &filePath) {
    ifstream fin(filePath.c_str());
    if (!fin.is_open()) { 
      return "";
    }
    ostringstream tmp;
    tmp << fin.rdbuf();
    fin.close();
    return tmp.str();
}

bool FileUtil::WriteLocalFile(const string &filePath, const string &content) {
    std::ofstream file(filePath.c_str());
    if (!file) {
        return false;
    }
    file.write(content.c_str(), content.length());
    file.flush();
    file.close();
    return true;
}

bool FileUtil::CompFile(const string& first, const string& second)
{
    return (CompFile(first.c_str(), second.c_str()) == 0);
}

bool FileUtil::IsFileExist(const std::string& file){
    if (access(file.c_str(), F_OK) == 0) {
        return true;
    }
    return false;
}

bool FileUtil::IsDirExist(const std::string& dir){
    struct stat st;
    if (stat(dir.c_str(), &st) != 0) {
        return false;
    }

    if (!S_ISDIR(st.st_mode)) {
        return false;
    }

    return true;
    
}


int FileUtil::CompFile(const char* first, const char* second)
{
    if (first == NULL || second == NULL)
    {
        return -1;
    }

    struct stat fStat, sStat;

    int ret;
    ret = lstat(first, &fStat);
    if (ret < 0)
    {
        return -1;
    }

    ret = lstat(second, &sStat);
    if (ret < 0)
    {
        return -1;
    }

    if (fStat.st_size != sStat.st_size)
    {
        return 1;
    }

    FILE *ff, *sf;
    ff = fopen(first, "rb");
    if (ff == NULL)
    {
        return -1;
    }

    sf = fopen(second, "rb");
    if (sf == NULL)
    {
        fclose(ff);
        return -1;
    }

    char fBuf[1024], sBuf[1024];
    int fc, sc, result = 0;
    while (1)
    {
        fc = fread(fBuf, 1, 1024, ff);
        if (fc == 0)
        {
            break;
        }

        sc = fread(sBuf, 1, 1024, sf);
        ret = memcmp(fBuf, sBuf, fc);
        if (ret != 0)
        {
            result = 1;
            break;
        }
    }

    fclose(sf);
    fclose(ff);
    return result;
}


std::string FileUtil::GetParentDir(const std::string &currentDir)
{
    if (currentDir.empty()) {
        return "";
    }
    
    size_t delimPos = string::npos;
    if (DIR_DELIM == *(currentDir.rbegin())) {
        //the last charactor is '/', then rfind from the next char
        delimPos = currentDir.rfind(DIR_DELIM, currentDir.size() - 2);
    } else {
        delimPos = currentDir.rfind(DIR_DELIM);
    }

    if (string::npos == delimPos) {
        //not found '/', than parent dir is null
        return "";
    }

    string parentDir = currentDir.substr(0, delimPos);
    

    return parentDir;
}


bool FileUtil::MakeLocalDir(const string &localDirPath, bool recursive)
{
    if (recursive) {
        return RecursiveMakeLocalDir(localDirPath);
    }
    
    if (IsDirExist(localDirPath)) {
        return false;
    }

    int mkdirRet = mkdir(localDirPath.c_str(), S_IRWXU);
    if (mkdirRet != 0) {
        return false;
    }
    return true;
}


bool FileUtil::RecursiveMakeLocalDir(const string &localDirPath) {
    if (localDirPath.empty()) {
        return false;
    } 

    if (IsDirExist(localDirPath)) {
        return true;
    }

    string currentDir = localDirPath;
    string parentDir = GetParentDir(currentDir);

    //create parent dir
    if (parentDir != "" && !IsDirExist(parentDir)) {
        bool ret = RecursiveMakeLocalDir(parentDir);
        if (!ret) {
            return false;
        }
    }

    //make current dir
    int mkdirRet = mkdir(currentDir.c_str(), S_IRWXU);
    if (mkdirRet != 0) {
        return false;
    }
    return true;
}


bool FileUtil::RemoveLocalDir(const std::string &localDirPath,
                              bool removeNoneEmptyDir)
{
    bool dirExist = FileUtil::IsDirExist(localDirPath);
    if (!dirExist) {
        return false;
    }
    
    if (removeNoneEmptyDir) {
        return RecursiveRemoveLocalDir(localDirPath);
    } else if (rmdir(localDirPath.c_str()) != 0) {
        return false;
    }
    return true;
}

bool FileUtil::RecursiveRemoveLocalDir(const std::string &localDirPath){
    DIR *dir = NULL;
    struct dirent *dp = NULL;

    if ((dir = opendir(localDirPath.c_str())) == NULL) {
        return false;
    }
    
    bool retFlag = false;
    while((dp = readdir(dir)) != NULL) {
        if (dp->d_type & DT_DIR) {
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                retFlag = true;
                continue;
            } else {
                string childDirPath = localDirPath + DIR_DELIM + string(dp->d_name);
                retFlag = RecursiveRemoveLocalDir(childDirPath);
            }
        } else {
            string childFilePath = localDirPath + DIR_DELIM + string(dp->d_name);
            retFlag = DeleteLocalFile(childFilePath);
        }

        if (!retFlag) {
            break;
        }
    }
    closedir(dir);

    //rm itself
    if (retFlag && rmdir(localDirPath.c_str()) != 0) {
        return false;
    }
    return retFlag;
}



bool FileUtil::CopyLocalFile(const string &desFilePath,
                             const string &srcFilePath)
{
    if (!IsFileExist(srcFilePath)) {
        return false;
    }
    
    //read content to stringstream
    ifstream in(srcFilePath.c_str());
    stringstream ss;
    string line;
    char c;
    while (in.get(c)) {
        ss << c;
    }
    in.close();

    //write content to desFilePath
    ofstream out(desFilePath.c_str());
    string content = ss.str();
    if (!out.write(content.c_str(), content.size())) {
        return false;
    }

    return true;
}

    
bool FileUtil::ListLocalDir(const string& dir, 
                            vector<string>& entryVec,
                            int flag)
{
    entryVec.clear();

    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        return false;
    }

    while ((dirp = readdir(dp)) != NULL) {
        string name = dirp->d_name;
        if (name == "." || name == "..") {
            continue;
        }
        if (DT_DIR == dirp->d_type) {
            if (flag == 0 || flag ==2 ){
               entryVec.push_back(string(dirp->d_name) ); 
            }
        } else {
            if(flag == 0 || flag == 1){
                entryVec.push_back(string(dirp->d_name));
            }
        }
    }
    closedir(dp);
    return true;
}

std::string FileUtil::GetAbsFile(std::string parentDir, std::string fileName){
    return parentDir + DIR_DELIM + fileName;
}
std::string FileUtil::GenerateAbsolutePath(const std::string &directory, const std::string &file_path)
{
    if (file_path.size() > 0) {
      if (file_path.at(0) == '/') {
        return file_path;
      }
      else {
        return directory + "/" + file_path;
      }
    }
    return "";

}

bool FileUtil::Parse(std::string& file, std::map<std::string,std::string>& kv)
{
  ifstream fp( file.c_str() );
  if ( !fp.is_open() ) {
    fprintf(stderr, "Can't open config file:%s\n", file.c_str());
    return false;
  }
  bool ret = true;
  string line, key, value;
  string trimedstr;
  string spliter = "=";
  while ( getline( fp, line ) ) {
    trimedstr  = StringUtil::TrimString(line);
    if(trimedstr == "") continue;
    if(trimedstr[0] == '#') continue;
    vector<string> tokens;
    StringUtil::SplitTokensByDelimiter(trimedstr, tokens, spliter.c_str(),spliter.size());
    if(tokens.size() != 2) {
        continue;
    }
    kv[StringUtil::TrimString(tokens[0])] = StringUtil::TrimString(tokens[1]);
  }

  fp.close();
  return ret;
}

int FileUtil::WriteLocalFileByDate(const std::string& filePath, 
                                   const std::string &content) 
{
  time_t now = time(NULL);
  char timenow[12] = {'\0'};
  strftime(timenow, sizeof(timenow), "%Y-%m-%d", localtime(&now));
  string curFileName = string(filePath);
  curFileName.append(".");
  curFileName.append(timenow);
  FILE *fp;
  if ((fp = fopen(curFileName.c_str(), "a+")) == NULL) {
    return -1; 
  }   
  int ret;
  if ((ret = fprintf(fp, "%s\n", content.c_str()) < 0)) {
  }   
  fclose(fp);
  return ret;
}


bool FileUtil::Rename(std::string oldfile, std::string newfile){
    if(0 != rename(oldfile.c_str(), newfile.c_str()))
        return false;
    return true;
}
SHARELIB_ES;

