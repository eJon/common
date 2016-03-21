#include <sharelib/util/string_util.h>
#include <stdlib.h>
#include <errno.h>
#include <algorithm>
#include <sharelib/common.h>
using namespace std;
SHARELIB_BS;
const std::string StringUtil::NULL_STRING = "";

static unsigned char gU8Mask[6] = { 0x80, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
static unsigned char gU8Mark[6] = { 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

StringUtil::StringUtil() { 
}

StringUtil::~StringUtil() { 
}

string StringUtil::TrimString(const std::string& s)
{
    static const char* whiteSpace = " \t\r\n";

    // test for null string
    if (s.empty())
        return s;

    // find first non-space character
    std::string::size_type b = s.find_first_not_of(whiteSpace);
    if (b == std::string::npos) // No non-spaces
        return "";

    // find last non-space character
    std::string::size_type e = s.find_last_not_of(whiteSpace);

    // return the remaining characters
    return std::string(s, b, e - b + 1);

}


void StringUtil::SplitTokensByDelimiter(
    const std::string& line,
    std::vector<std::string>& tokens,
    const char* delim,
    size_t size)
{
    tokens.clear();
    std::string::size_type startPos = 0;
    while (startPos != std::string::npos)
    {
        std::string::size_type endPos = line.find(delim, startPos, size);
        if (endPos == std::string::npos)
        {
            tokens.push_back(line.substr(startPos));
            startPos = std::string::npos;
        }
        else if (startPos == line.length())
        {
            tokens.push_back(std::string(""));
            startPos = std::string::npos;
        }
        else
        {
            tokens.push_back(line.substr(startPos, endPos - startPos));
            startPos = endPos + size;
        }
    }
}

/* valid utf-8 format
 * 0xxxxxxx                                        (00-7f）
 * 110xxxxx 10xxxxxx                               (c0-df)(80-bf)
 * 1110xxxx 10xxxxxx 10xxxxxx                      (e0-ef)(80-bf)(80-bf)
 * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx              (f0-f7)(80-bf)(80-bf)(80-bf)
 * 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx     (f8-fb)(80-bf)(80-bf)(80-bf)(80-bf)
 * 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx (fc-fd)(80-bf)(80-bf)(80-bf)(80-bf)(80-bf)
 * Checking the string from 3 points.
 * 1. A token has 1-6 bytes.
 * 2. In a token, the first 2 bits of the bytes except the first byte should be 10xxxxx.
 * 3. The count of the bytes should follow the first byte.
 */
bool StringUtil::IsValidUTF8(const std::string& str)
{
    std::string::size_type i, s;
    for (i = 0U, s = 0U; i < str.length(); ++i)
    {
        unsigned char value = str[i];
        if (s == 0)
        {
            for (; s < 6U && ((value & gU8Mask[s]) != gU8Mark[s]); ++s)
            {}

            if (s == 6U)
            {
                return false;
            }
        }
        else
        {
            if ((value & 0xC0) != 0x80)
            {
                return false;
            }

            --s;
        }
    }

    return (s == 0U);
}

unsigned int StringUtil::GetCountOfCharUTF8(const std::string& str)
{
    unsigned int count = 0;
    for (std::string::size_type i = 0U; i < str.length();)
    {
        unsigned char value = str[i];
        unsigned int j;
        for (j = 0U; j < 6U; ++j)
        {
            if ((value & gU8Mask[j]) == gU8Mark[j])
            {
                i = i + j + 1;
                ++count;
                break;
            }
        }

        if (j == 6U)
        {
            return 0;
        }
    }

    return count;
}

unsigned int StringUtil::GetCountOfLatinCharUTF8(const std::string& str)
{
    unsigned int count = 0;
    for (std::string::size_type i = 0U; i < str.length();)
    {
        unsigned char value = str[i];
        unsigned int j;
        for (j = 0U; j < 6U; ++j)
        {
            if ((value & gU8Mask[j]) == gU8Mark[j])
            {
                i = i + j + 1;
                if (j == 0U)
                {
                    ++count;
                }

                break;
            }
        }

        if (j == 6U)
        {
            return 0;
        }
    }

    return count;
}

std::string StringUtil::GetNextCharUTF8(
    const std::string& str,
    std::string::size_type start)
{
    if (start < str.length())
    {
        unsigned char value = str[start];
        unsigned int i;
        for (i = 0U; i < 6U; ++i)
        {
            if ((value & gU8Mask[i]) == gU8Mark[i])
            {
                if (start + i < str.length())
                {
                    return str.substr(start, i + 1);
                }
                else
                {
                    break;
                }
            }
        }
    }

    return "";
}

unsigned int StringUtil::CalEditDistanceUTF8(
    const std::string& first,
    const std::string& second)
{
    unsigned int n = GetCountOfCharUTF8(first);
    unsigned int m = GetCountOfCharUTF8(second);
    unsigned int* value[2];
    value[0] = new unsigned int[m + 1];
    value[1] = new unsigned int[m + 1];

    for (unsigned int i = 0; i <= m; ++i)
    {
        value[0][i] = i;
    }

    std::string::size_type iStart, jStart;
    iStart = 0;
    int index = 1;
    for (unsigned int i = 1; i <= n; ++i)
    {
        value[index][0] = i;
        std::string nextI = GetNextCharUTF8(first, iStart);
        iStart += nextI.length();
        jStart = 0;

        unsigned int preIndex = (index + 1) % 2;
        for (unsigned int j = 1; j <= m; ++j)
        {
            std::string nextJ = GetNextCharUTF8(second, jStart);
            jStart += nextJ.length();
            unsigned delta = (nextI == nextJ ? 0 : 1);

            if (value[preIndex][j] < value[index][j - 1])
            {
                value[index][j] = value[preIndex][j] + 1;
            }
            else
            {
                value[index][j] = value[index][j - 1] + 1;
            }

            if (value[index][j] > value[preIndex][j - 1] + delta)
            {
                value[index][j] = value[preIndex][j - 1] + delta;
            }
        }

        index = preIndex;
    }

    unsigned int result = value[(index + 1) % 2][m];
    delete[] value[0];
    delete[] value[1];
    return result;
}

/////////////////////////////////////////////////////////////////
void StringUtil::Trim(std::string& str) {
    str.erase(str.find_last_not_of(' ') + 1);
    str.erase(0, str.find_first_not_of(' '));
}

bool StringUtil::StartsWith(const std::string &str, const std::string &prefix) {
    return (str.size() >= prefix.size()) 
        && (str.compare(0, prefix.size(), prefix) == 0); 
}

bool StringUtil::EndsWith(const std::string &str, const std::string &suffix) {
    size_t s1 = str.size();
    size_t s2 = suffix.size();
    return (s1 >= s2) && (str.compare(s1 - s2, s2, suffix) == 0);
}

std::vector<std::string> StringUtil::Split(const std::string& text, const std::string &sepStr, bool ignoreEmpty)
{
    std::vector<std::string> vec;
    std::string str(text);
    std::string sep(sepStr);
    size_t n = 0, old = 0;
    while (n != std::string::npos)
    {
        n = str.find(sep,n);
        if (n != std::string::npos)
        {
            if (!ignoreEmpty || n != old) 
                vec.push_back(str.substr(old, n-old));
            n += sep.length();
            old = n;
        }
    }

    if (!ignoreEmpty || old < str.length()) {
        vec.push_back(str.substr(old, str.length() - old));
    }
    return vec;
}


void StringUtil::Split(const std::string& text, const std::string &sepStr,
      std::vector<uint32_t>& dest_vec)
{
    std::string str(text);
    std::string sep(sepStr);
    size_t n = 0, old = 0;
    uint32_t code = 0;
    while (n != std::string::npos)
    {
        n = str.find(sep,n);
        if (n != std::string::npos)
        {
          if (!StrToUInt32(TrimString(str.substr(old, n-old)).c_str(), code)) {
            continue;
          }
          dest_vec.push_back(code);
          n += sep.length();
          old = n;
        }
    }
    if (old < str.length()) {
        if (StrToUInt32(TrimString(str.substr(old, str.length() - old)).c_str(), code)) {
          dest_vec.push_back(code);
        }
    }
    return;
}


bool StringUtil::IsSpace(const string& text) {
    if (text == string("　")) {
        return true;
    }
    if (text.length() > 1) {
        return false;
    }
    return isspace(text[0]);
}

bool StringUtil::IsSpace(const ShortString& text) {
    if (text == "　") {
        return true;
    }
    
    if (text.length() > 1) {
        return false;
    }
    
    return isspace(text[0]);
}

void StringUtil::ToUpperCase(char *str) {
    if (str) {
        while (*str) {
            if (*str >= 'a' && *str <= 'z') {
                *str += 'A' - 'a';
            }
            str++;
        }
    }
}

void StringUtil::ToUpperCase(string &str) {
    for(size_t i = 0; i < str.size(); i++) {
        str[i] = toupper(str[i]);
    }
}

void StringUtil::ToUpperCase(const char *str, std::string &retStr) {
    retStr = str;
    for(size_t i = 0; i < retStr.size(); i++) {
        retStr[i] = toupper(str[i]);
    }
}

void StringUtil::ToLowerCase(string &str) {
    for(size_t i = 0; i < str.size(); i++) {
        str[i] = tolower(str[i]);
    }
}

void StringUtil::ToLowerCase(char *str) {
    if (str) {
        while (*str) {
            if (*str >= 'A' && *str <= 'Z') {
                *str -= 'A' - 'a';
            }
            str++;
        }
    }
}

void StringUtil::ToLowerCase(const char *str, std::string &retStr) {
    retStr = str;
    ToLowerCase(retStr);
}

bool StringUtil::StrToInt8(const char* str, int8_t& value)
{
    int32_t v32;
    bool ret = StrToInt32(str, v32);
    if (ret)
    {
        if (v32 >= INT8_MIN && v32 <= INT8_MAX)
        {
            value = (int8_t)v32;
            return true;
        }
    }
    return false;
}

bool StringUtil::StrToUInt8(const char* str, uint8_t& value)
{
    uint32_t v32;
    bool ret = StrToUInt32(str, v32);
    if (ret)
    {
        if (v32 <= UINT8_MAX)
        {
            value = (uint8_t)v32;
            return true;
        }
    }
    return false;
}

bool StringUtil::StrToInt16(const char* str, int16_t& value)
{
    int32_t v32;
    bool ret = StrToInt32(str, v32);
    if (ret)
    {
        if (v32 >= INT16_MIN && v32 <= INT16_MAX)
        {
            value = (int16_t)v32;
            return true;
        }
    }
    return false;
}

bool StringUtil::StrToUInt16(const char* str, uint16_t& value)
{
    uint32_t v32;
    bool ret = StrToUInt32(str, v32);
    if (ret)
    {
        if (v32 <= UINT16_MAX)
        {
            value = (uint16_t)v32;
            return true;
        }
    }
    return false;
}

bool StringUtil::StrToInt32(const char* str, int32_t& value) 
{
    if (NULL == str || *str == 0) 
    {
        return false;
    }
    char* endPtr = NULL;
    errno = 0;

# if __WORDSIZE == 64
    int64_t value64 = strtol(str, &endPtr, 10);
    if (value64 < INT32_MIN || value64 > INT32_MAX)
    {
        return false;
    }
    value = (int32_t)value64;
# else
    value = (int32_t)strtol(str, &endPtr, 10);
# endif

    if (errno == 0 && endPtr && *endPtr == 0) 
    {
        return true;
    }
    return false;
}

bool StringUtil::StrToUInt32(const char* str, uint32_t& value) 
{
    if (NULL == str || *str == 0 || *str == '-') 
    {
        return false;
    }
    char* endPtr = NULL;
    errno = 0;

# if __WORDSIZE == 64
    uint64_t value64 = strtoul(str, &endPtr, 10);
    if (value64 > UINT32_MAX)
    {
        return false;
    }
    value = (int32_t)value64;
# else
    value = (uint32_t)strtoul(str, &endPtr, 10);
# endif

    if (errno == 0 && endPtr && *endPtr == 0) 
    {
        return true;
    }
    return false;
}

bool StringUtil::StrToUInt64(const char* str, uint64_t& value)
{
    if (NULL == str || *str == 0 || *str == '-') 
    {
        return false;
    }
    char* endPtr = NULL;
    errno = 0;
    value = (uint64_t)strtoull(str, &endPtr, 10);
    if (errno == 0 && endPtr && *endPtr == 0) 
    {
        return true;
    }
    return false;
}

bool StringUtil::StrToInt64(const char* str, int64_t& value) 
{
    if (NULL == str || *str == 0) 
    {
        return false;
    }
    char* endPtr = NULL;
    errno = 0;
    value = (int64_t)strtoll(str, &endPtr, 10);
    if (errno == 0 && endPtr && *endPtr == 0) 
    {
        return true;
    }
    return false;
}

bool StringUtil::HexStrToUint128(const char* str, uint128_t& value)
{
    char buf[17];
    buf[16] = 0;
    memcpy(buf, str, 16);
    HexStrToUint64(buf, value.value[0]);

    memcpy(buf, str + 16, 16);
    HexStrToUint64(buf, value.value[1]);
    return true;
}

bool StringUtil::HexStrToUint64(const char* str, uint64_t& value) 
{
    if (NULL == str || *str == 0) 
     {
         return false;
     }
     char* endPtr = NULL;
     errno = 0;
     value = (uint64_t)strtoull(str, &endPtr, 16);
     if (errno == 0 && endPtr && *endPtr == 0) 
     {
         return true;
     }
     return false;
}

uint32_t StringUtil::DeserializeUInt32(const std::string& str)
{
    assert(str.length() == sizeof(uint32_t));

    uint32_t value= 0;
    for (size_t i = 0; i < str.length(); ++i)
    {
        value <<= 8;
        value |= (unsigned char)str[i];
    }
    return value;
}

void StringUtil::SerializeUInt32(uint32_t value, std::string& str)
{
    char key[4];
    for (int i = (int)sizeof(uint32_t) - 1; i >= 0; --i) 
    {
        key[i] = (char)(value & 0xFF);
        value >>= 8;
    }
    str.assign(key, sizeof(uint32_t));
}

void StringUtil::SerializeUInt64(uint64_t value, std::string& str)
{
    char key[8];
    for (int i = (int)sizeof(uint64_t) - 1; i >= 0; --i) 
    {
        key[i] = (char)(value & 0xFF);
        value >>= 8;
    }
    str.assign(key, sizeof(uint64_t));
}

uint64_t  StringUtil::DeserializeUInt64(const std::string& str)
{
    assert(str.length() == sizeof(uint64_t));

    uint64_t value= 0;
    for (size_t i = 0; i < str.length(); ++i)
    {
        value <<= 8;
        value |= (unsigned char)str[i];
    }
    return value;
}

void StringUtil::SerializeUInt128(uint128_t value, std::string& str)
{   
    char key[16];
    int i = (int)sizeof(uint64_t) * 2 - 1;
    for (; i >= (int)sizeof(uint64_t); --i)
    {
        key[i] = (char)(value.value[1] & 0xFF);
        value.value[1] >>= 8;
    }

    for (; i >= 0; --i)
    {
        key[i] = (char)(value.value[0] & 0xFF);
        value.value[0] >>= 8;
    }

    str.assign(key, 16);
}

uint128_t StringUtil::DeserializeUInt128(const std::string& str)
{
    assert(str.length() == sizeof(uint64_t) * 2);

    uint128_t value= 0;
    size_t i = 0;

    for (; i < sizeof(uint64_t); ++i)
    {
        value.value[0] <<= 8;
        value.value[0] |= (unsigned char)str[i];
    }

    for (; i < sizeof(uint64_t) * 2; ++i)
    {
        value.value[1] <<= 8;
        value.value[1] |= (unsigned char)str[i];
    }

    return value;
}

bool StringUtil::StrToFloat(const char* str, float& value) 
{
    if (NULL == str || *str == 0) 
    {
        return false;
    }
    errno = 0;
    char* endPtr = NULL;
    value = strtof(str, &endPtr);
    if (errno == 0 && endPtr && *endPtr == 0) 
    {
        return true;
    }
    return false;
}

bool StringUtil::StrToDouble(const char* str, double& value) 
{
    if (NULL == str || *str == 0) 
    {
        return false;
    }
    errno = 0;
    char* endPtr = NULL;
    value = strtod(str, &endPtr);
    if (errno == 0 && endPtr && *endPtr == 0) 
    {
        return true;
    }
    return false;
}

void StringUtil::Uint128ToHexStr(uint128_t& value, char* hexStr, int len)
{
    assert(len > 32);
    for(int i = 0; i < value.Count(); i++)
    {
        Uint64ToHexStr(value.value[i], hexStr, 17);
        hexStr += 16;
    }
}

void StringUtil::Uint64ToHexStr(uint64_t value, char* hexStr, int len)
{
    assert(len > 16);
    snprintf(hexStr, len, "%016lx", value);
}

int8_t StringUtil::StrToInt8WithDefault(const char* str, int8_t defaultValue)
{
    int8_t tmp;
    if(StrToInt8(str, tmp))
    {
        return tmp;
    }
    return defaultValue;
}

uint8_t StringUtil::StrToUInt8WithDefault(const char* str, uint8_t defaultValue)
{
    uint8_t tmp;
    if(StrToUInt8(str, tmp))
    {
        return tmp;
    }
    return defaultValue;
}

int16_t StringUtil::StrToInt16WithDefault(const char* str, int16_t defaultValue)
{
    int16_t tmp;
    if(StrToInt16(str, tmp))
    {
        return tmp;
    }
    return defaultValue;
}

uint16_t StringUtil::StrToUInt16WithDefault(const char* str, uint16_t defaultValue)
{
    uint16_t tmp;
    if(StrToUInt16(str, tmp))
    {
        return tmp;
    }
    return defaultValue;
}

int32_t StringUtil::StrToInt32WithDefault(const char* str, int32_t defaultValue)
{
    int32_t tmp;
    if(StrToInt32(str, tmp))
    {
        return tmp;
    }
    return defaultValue;
}

uint32_t StringUtil::StrToUInt32WithDefault(const char* str, uint32_t defaultValue)
{
    uint32_t tmp;
    if(StrToUInt32(str, tmp))
    {
        return tmp;
    }
    return defaultValue;
}

int64_t StringUtil::StrToInt64WithDefault(const char* str, int64_t defaultValue)
{
    int64_t tmp;
    if(StrToInt64(str, tmp))
    {
        return tmp;
    }
    return defaultValue;
}

uint64_t StringUtil::StrToUInt64WithDefault(const char* str, uint64_t defaultValue)
{
    uint64_t tmp;
    if(StrToUInt64(str, tmp))
    {
        return tmp;
    }
    return defaultValue;
}

float StringUtil::StrToFloatWithDefault(const char* str, float defaultValue)
{
    float tmp;
    if(StrToFloat(str, tmp))
    {
        return tmp;
    }
    return defaultValue;
}

double StringUtil::StrToDoubleWithDefault(const char* str, double defaultValue)
{
    double tmp;
    if(StrToDouble(str, tmp))
    {
        return tmp;
    }
    return defaultValue;
}

void StringUtil::SundaySearch(const string &text, const string &key, vector<size_t> &posVec, bool caseSensitive) {
    size_t textSize = text.size();
    size_t keySize = key.size();   
    if (caseSensitive) {
        SundaySearch(text.c_str(), textSize, key.c_str(), keySize, posVec);
    } else {
        string str(text);
        string keyword(key);
        ToUpperCase(str);
        ToUpperCase(keyword);
        SundaySearch(str.c_str(), textSize, keyword.c_str(), keySize, posVec);        
    }
}

void StringUtil::SundaySearch(const char *text, const char *key, vector<size_t> &posVec) {
    size_t textSize = strlen(text);
    size_t keySize = strlen(key);
    SundaySearch(text, textSize, key, keySize, posVec);
}

void StringUtil::SundaySearch(const char *text, size_t textSize, const char *key, size_t keySize, vector<size_t> &posVec) {
    posVec.clear();
    if (textSize < keySize || keySize == 0) {
        return;
    }

    if (keySize == 1) {
        for (size_t i = 0; i < textSize; ++i) {
            if (text[i] == *key) {
                posVec.push_back(i);
            }
        }
        return;
    }
    
    uint32_t next[256];
    for(size_t i = 0; i < 256; ++i) {
        next[i] = keySize + 1;   
    }
    
    for(size_t i = 0; i < keySize; ++i) {
        next[(unsigned char)(key[i])] = keySize - i;  
    }
    
    size_t maxPos = textSize - keySize;
    for(size_t pos = 0; pos <= maxPos;) {   
        size_t i;
        size_t j;
        for(i = pos, j = 0; j < keySize; ++j, ++i) {    
            if(text[i] != key[j]) {   
                pos += next[(unsigned char)(text[pos + keySize])];
                break;   
            }   
        }   
        if(j == keySize) {
            posVec.push_back(pos);
            pos += next[(unsigned char)(text[pos + keySize])];
        }
    }   
}

void StringUtil::ReplaceLast(string &str, const string& oldStr, 
                             const string& newStr) 
{
    size_t pos = str.rfind(oldStr);
    if (pos != string::npos) {
        str.replace(pos, oldStr.size(), newStr);
    }
}

void StringUtil::ReplaceAll(string& str, const string& oldStr,
                            const string& newStr) {
    while (str.find(oldStr) != string::npos) {
        ReplaceLast(str, oldStr, newStr);
    }
}

void StringUtil::Replace(std::string &str, char oldValue, char newValue) {
    ::replace(str.begin(), str.end(), oldValue, newValue);
}
SHARELIB_ES;

