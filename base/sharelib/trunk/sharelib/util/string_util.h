#ifndef SHARELIB_UTIL_STRINGUTIL_H
#define SHARELIB_UTIL_STRINGUTIL_H
#include <assert.h>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <sharelib/util/short_string.h>
#include <sharelib/util/long_hash_value.h>
#include <sharelib/common.h>
SHARELIB_BS;

class StringUtil
{
public:
    static const std::string NULL_STRING;
    typedef bool (*CMP_CHAR_FUNC)(const char a, const char b);
public:
    StringUtil();
    ~StringUtil();
public:

    static std::string TrimString(const std::string& s);

    static void SplitTokensByDelimiter(
        const std::string& line,
        std::vector<std::string>& tokens,
        const char* delim,
        size_t size);

    /* valid utf-8 format
     * 0xxxxxxx                                         (00-7f）
     * 110xxxxx 10xxxxxx                                (c0-df)(80-bf)
     * 1110xxxx 10xxxxxx 10xxxxxx                       (e0-ef)(80-bf)(80-bf)
     * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx              (f0-f7)(80-bf)(80-bf)(80-bf)
     * 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx     (f8-fb)(80-bf)(80-bf)(80-bf)(80-bf)
     * 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (fc-fd)(80-bf)(80-bf)(80-bf)(80-bf)(80-bf)
     * Checking the string from 3 points.
     * 1. A token has 1-6 bytes.
     * 2. In a token, the first 2 bits of the bytes except the first byte should be 10xxxxx.
     * 3. The count of the bytes should follow the first byte.
     */
     static bool IsValidUTF8(const std::string& str);

     static unsigned int GetCountOfCharUTF8(const std::string& str);

     static unsigned int GetCountOfLatinCharUTF8(const std::string& str);

     static std::string GetNextCharUTF8(const std::string& str, std::string::size_type start);

    static unsigned int CalEditDistanceUTF8(const std::string& first, const std::string& second);
    
public:
    static void Trim(std::string& str);
    static bool StartsWith(const std::string &str, const std::string &prefix);
    static bool EndsWith(const std::string &str, const std::string &suffix);

    static void Split(const std::string& text, const std::string &sepStr, std::vector<uint32_t>& dest_vec);

    static std::vector<std::string> Split(const std::string& text, const std::string &sepStr, bool ignoreEmpty = true);
    static bool IsSpace(const std::string& text);
    static bool IsSpace(const ShortString& text);

    static void ToUpperCase(char *str);
    static void ToUpperCase(const char *str, std::string &retStr);
    static void ToUpperCase(std::string &str);
    static void ToLowerCase(char *str);
    static void ToLowerCase(std::string &str);
    static void ToLowerCase(const char *str, std::string &retStr);

    static bool StrToInt8(const char* str, int8_t& value);
    static bool StrToUInt8(const char* str, uint8_t& value);
    static bool StrToInt16(const char* str, int16_t& value);
    static bool StrToUInt16(const char* str, uint16_t& value);
    static bool StrToInt32(const char* str, int32_t& value);
    static bool StrToUInt32(const char* str, uint32_t& value);
    static bool StrToInt64(const char* str, int64_t& value);
    static bool StrToUInt64(const char* str, uint64_t& value);
    static bool StrToFloat(const char *str, float &value);
    static bool StrToDouble(const char *str, double &value);
    static bool HexStrToUint64(const char* str, uint64_t& value);
    static void Uint64ToHexStr(uint64_t value, char* hexStr, int len);

    static uint32_t DeserializeUInt32(const std::string& str);
    static void SerializeUInt32(uint32_t value, std::string& str);

    static uint64_t DeserializeUInt64(const std::string& str);
    static void SerializeUInt64(uint64_t value, std::string& str);

    static uint128_t DeserializeUInt128(const std::string& str);
    static void SerializeUInt128(uint128_t value, std::string& str);

    static int8_t StrToInt8WithDefault(const char* str, int8_t defaultValue);
    static uint8_t StrToUInt8WithDefault(const char* str, uint8_t defaultValue);
    static int16_t StrToInt16WithDefault(const char* str, int16_t defaultValue);
    static uint16_t StrToUInt16WithDefault(const char* str, uint16_t defaultValue);
    static int32_t StrToInt32WithDefault(const char* str, int32_t defaultValue);
    static uint32_t StrToUInt32WithDefault(const char* str, uint32_t defaultValue);
    static int64_t StrToInt64WithDefault(const char* str, int64_t defaultValue);
    static uint64_t StrToUInt64WithDefault(const char* str, uint64_t defaultValue);
    static float StrToFloatWithDefault(const char* str, float defaultValue);
    static double StrToDoubleWithDefault(const char* str, double defaultValue);

    static bool HexStrToUint128(const char* str, uint128_t& value);
    static void Uint128ToHexStr(uint128_t& value, char* hexStr, int len);
    
    static void Replace(std::string &str, char oldValue, char newValue);
    static void ReplaceLast(std::string &str, const std::string &oldStr, 
                            const std::string &newStr);
    static void ReplaceAll(std::string& str, const std::string& oldStr,
                                const std::string& newStr);
    
    static void SundaySearch(const std::string &text, 
                             const std::string &key, 
                             std::vector<size_t> &posVec, 
                             bool caseSensitive = true);   
    static void SundaySearch(const char *text, const char *key, 
                             std::vector<size_t> &posVec);   
    static void SundaySearch(const char *text, size_t textSize, 
                             const char *key, size_t keySize, 
                             std::vector<size_t> &posVec);

    static const std::string& GetValueFromMap(const std::string& key,
            const std::map<std::string, std::string> &map);

    template<typename T>
    static std::string ToString(const T &x);
    
    template<typename T>
    static T NumberFromString(const std::string &str);

    template<typename T>
    static T FromString(const std::string &str);

    template<typename T>
    static void FromString(const std::vector<std::string> &strVec, std::vector<T> &vec);

    template<typename T>
    static void FromString(const std::string &str, std::vector<T> &vec, const std::string &delim);

    template<typename T>
    static void FromString(const std::string &str, std::vector<std::vector<T> > &vec, const std::string &delim, const std::string &delim2);

    template<typename T>
    static std::string ToString(const std::vector<T> &x, const std::string &delim = " ");

    template<typename T>
    static std::string ToString(const std::vector<std::vector<T> > &x, const std::string &delim1, const std::string &delim2);
};

template<typename T>
inline T StringUtil::NumberFromString(const std::string &str) {
    if (str.size() > 2) {
        size_t pos = 0;
        if (str[0] == '-') {
            pos = 1;
        }
        char ch1 = str[pos];
        char ch2 = str[pos + 1];
        if (ch1 == '0' && (ch2 == 'x' || ch2 == 'X')) {
            long long value = 0;
            sscanf(str.c_str(), "%llx", &value);
            return (T) value;
        }
    }  

    return FromString<T>(str);
}

template<typename T>
inline T StringUtil::FromString(const std::string& str) {
    T ret = T();
    std::istringstream iss(str);
    iss >> ret;
    return ret;    
}

template<typename T>
inline std::string StringUtil::ToString(const T &x) {
    std::ostringstream oss;
    oss << x;
    return oss.str();    
}

template<> 
inline std::string StringUtil::ToString<int8_t>(const int8_t &x) {
    return ToString<int32_t>(int32_t(x));
}

template<> 
inline std::string StringUtil::ToString<uint8_t>(const uint8_t &x) {
    return ToString<uint32_t>(uint32_t(x));
}

template<typename T>
inline std::string StringUtil::ToString(const std::vector<T> &x, const std::string &delim) {
    std::ostringstream oss; 
    for (typename std::vector<T>::const_iterator it = x.begin();
         it != x.end(); ++it)
    {
        if (it != x.begin()) oss << delim;
        oss << (*it);
    }
    return oss.str();
}

template<typename T>
inline std::string StringUtil::ToString(const std::vector<std::vector<T> > &x, 
                                        const std::string &delim1,
                                        const std::string &delim2) {
    std::vector<std::string> strVec;
    for (typename std::vector<std::vector<T> >::const_iterator it = x.begin();
         it != x.end(); ++it)
    {
        strVec.push_back(ToString(*it, delim1));
    }    
    return ToString(strVec, delim2);
}

template<>
inline std::string StringUtil::FromString<std::string>(const std::string& str) {
    return str;
}

template<>
inline int8_t StringUtil::FromString<int8_t>(const std::string& str) {
    return (int8_t)FromString<int32_t>(str);
}

template<>
inline uint8_t StringUtil::FromString<uint8_t>(const std::string& str) {
    return (uint8_t)FromString<uint32_t>(str);
}

template<typename T>
inline void StringUtil::FromString(const std::vector<std::string> &strVec, std::vector<T> &vec) {
    vec.clear();
    vec.reserve(strVec.size());
    for (uint32_t i = 0; i < strVec.size(); ++i) {
        vec.push_back(FromString<T>(strVec[i]));
    }
}

template<typename T>
inline void StringUtil::FromString(const std::string &str, std::vector<T> &vec, const std::string &delim) {
    std::vector<std::string> strVec = Split(str, delim);
    FromString(strVec, vec);
}

template<typename T>
inline void StringUtil::FromString(const std::string &str, std::vector<std::vector<T> > &vec, const std::string &delim1, const std::string &delim2) {
    vec.clear();
    std::vector<std::string> strVec;
    FromString(str, strVec, delim2);
    vec.resize(strVec.size());
    for (uint32_t i = 0; i < strVec.size(); ++i) {
        FromString(strVec[i], vec[i], delim1);
    }
}

inline const std::string& StringUtil::GetValueFromMap(const std::string& key,
        const std::map<std::string, std::string>& map) 
{
    std::map<std::string, std::string>::const_iterator it = map.find(key);
    if (it != map.end()) {
        return it->second;
    }
    return NULL_STRING;
}

SHARELIB_ES;

#endif 
