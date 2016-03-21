#include <sharelib/util/filekv_parser.h>
#include <fstream>
#include <sharelib/util/string_util.h>
using namespace std;
SHARELIB_BS;

FilekvParser::FilekvParser(){
}

FilekvParser::~FilekvParser() { 
}

bool FilekvParser::Parse(std::string file)
{
    ifstream fp( file.c_str() );
    if ( !fp.is_open() ) {
        fprintf(stderr, "Can't open config file:%s\n", file.c_str());
        return false;
    }

    string line, key, value;
    while ( getline( fp, line ) ) {
        uint32_t i = 0;
        while ( isspace(line[i]) == true && i < line.length() ) ++i;
        if ( i >= line.length() ) {
            continue;
        } else {
            if ( line[i] == '#' )
                continue;
        }
    
        key.clear();
        value.clear();
        stringstream   linestream(line);
        getline( linestream, key, '=');
        linestream  >> value;
    
        if ( key.empty() || value.empty() ) {
            fprintf( stderr, "Invalid config:%s\n", line.c_str() );
            continue;
        }
    
        kv[StringUtil::TrimString(key)] = StringUtil::TrimString(value);
    }
  
    fp.close();
    return true;
}



int64_t FilekvParser::GetInt64(std::string key, bool& isExist, bool& isConvert){
    
    string ret = GetStr(key, isExist);
    if(!isExist) {
        return -1;
    }else{
        int64_t retint;
        isConvert= StringUtil::StrToInt64(ret.c_str(), retint);
        return retint;
    }
    
}
std::string  FilekvParser::GetStr(std::string key, bool& isExist){
    MapIter it = kv.find(key);
    isExist = (!(it == kv.end())) ;
    if(isExist) {
        return it->second;
    }else{
        return "";
    }
}
SHARELIB_ES;

