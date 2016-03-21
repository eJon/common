#ifndef SHARELIB_UTIL_FILEKV_PARSER_H
#define SHARELIB_UTIL_FILEKV_PARSER_H

#include <sharelib/util/log.h>
#include <sharelib/common.h>


SHARELIB_BS;
class FilekvParser
{
public:
    typedef std::map<std::string,std::string>::const_iterator MapIter;
public:
    FilekvParser();
    ~FilekvParser();
public:
    bool Parse(std::string file);

    int64_t GetInt64(std::string key, bool& isExist, bool& isConvert);
    std::string  GetStr(std::string key, bool& isExist);
private:
    std::map<std::string,std::string> kv;
};
SHARELIB_ES;

#endif //
