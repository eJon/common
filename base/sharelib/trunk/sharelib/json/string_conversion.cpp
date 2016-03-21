#include <string>
#include <inttypes.h>
#include <stdio.h>

#include "sharelib/json/string_tools.h"
#include "sharelib/json/exception.h"

SHARELIB_BS;

const std::string TRUE_STRING = "true";
const std::string FALSE_STRING = "false";
const uint32_t MAX_FLOAT_PRECISION = 7;
const uint32_t MAX_DOUBLE_PRECISION = 16;

/** Quote and un-quote string (json style) **/
void QuoteString(const std::string& textToEscape, std::string& ret)
{
    
    std::string::const_iterator last = textToEscape.begin();
    for (std::string::const_iterator it = textToEscape.begin();
         it != textToEscape.end();
         ++it
        )
    {
        /**
         * Escape the following one byte to corresponding quoted version (json-style) and do nothing on other cases
         *
         * '"', '\\', '/', '\b', '\f', '\n', '\r', '\t'
         */
        char escape;
        switch (*it)
        {
            case '"':  escape = '\"'; break;
            case '\\': escape = '\\'; break;
            case '/':  escape = '/'; break;
            case '\b': escape = 'b'; break;
            case '\f': escape = 'f';  break;
            case '\n': escape = 'n';  break;
            case '\r': escape = 'r';  break;
            case '\t': escape = 't'; break;
            default:
                continue;
        }
        ret.append(last, it);
        ret.push_back('\\');
        ret.push_back(escape);
        last = it + 1;
    }
    ret.append(last, textToEscape.end());
}

std::string QuoteString(const std::string& textToEscape)
{
    std::string ret;
    QuoteString(textToEscape, ret);
    return ret;
}

char ToHexDigit(uint8_t data)
{
    data = data & 0x0F;
    return static_cast<char>(data < 10 ? ('0' + data) : ('A' + data - 10));
}


std::string UnquoteString(const std::string& escapedText)
{
    a_IstreamBuffer s(escapedText);
    std::string str = UnquoteString(s);
    if (s.eof()) return str;
    throw BadQuoteString("\" should be represented as \\\"");
}

/// allow tailing unknow chars
void UnquoteString(const std::string& escapedText, size_t& pos, std::string& ret)
{
    bool escaped = false;
    for (; pos < escapedText.size(); ++pos)
    {
        char byte = escapedText[pos];
        if (escaped)
        {
            switch (byte)
            {
                case '"': byte = '"'; break;
                case '\\': byte = '\\'; break;
                case '/': byte = '/'; break;
                case 'b': byte = '\b'; break;
                case 'f': byte = '\f'; break;
                case 'n': byte = '\n'; break;
                case 'r': byte = '\r'; break;
                case 't': byte = '\t'; break;
                case 'u':
                {
                    uint16_t data = 0;
                    ++pos;
                    if (pos + 4 > escapedText.size())
                    {
                        SHARELIB_JSON_THROW(BadQuoteString, "\\u needs 4 digits followed");
                    }
                    for(int j = 0; j < 4; ++j, ++pos)
                    {
                        char b = escapedText[pos];
                        if (b >= '0' && b <='9')
                        {
                            data = (data<<4) | (0xF & (b-'0'));
                        }
                        else if (b>='a' && b<='f')
                        {
                            data = (data<<4) | (0xF & (b-'a'+10));
                        }
                        else if (b>='A' && b<='F')
                        {
                            data = (data<<4) | (0xF & (b-'A'+10));
                        }
                        else
                        {
                            SHARELIB_JSON_THROW(BadQuoteString, std::string("See unknown char after \\u:") + b);
                        }
                    }
                    ret.append(UnicodeToUtf8(data));
                    break;
                }
                default:
                    SHARELIB_JSON_THROW(BadQuoteString, "unknown char after \\");
            }
            if (byte != 'u') ret.push_back(byte);
            escaped = false;
        }
        else
        {   // escaped = false here
            if (byte == '\\') escaped = true;
            else if (byte == '"')
            {
                /// " should be escaped as \". An orphan " is not regarded as a part of the string
                /// itself.
                return;
            }
            else ret.push_back(byte);
        }
    }
    if (escaped)
    {
        SHARELIB_JSON_THROW(BadQuoteString, "Missing char after \\");
    }
}

std::string UnquoteString(a_Istream &is)
{
    bool escaped = false;
    std::string str;
    for(char byte = static_cast<char>(is.peek());
        is.good();
        is.get(), byte = static_cast<char>(is.peek()))
    {
        if (escaped)
        {
            switch (byte)
            {
                case '"': byte = '"'; break;
                case '\\': byte = '\\'; break;
                case '/': byte = '/'; break;
                case 'b': byte = '\b'; break;
                case 'f': byte = '\f'; break;
                case 'n': byte = '\n'; break;
                case 'r': byte = '\r'; break;
                case 't': byte = '\t'; break;
                case 'u':
                {
                    uint16_t data = 0;
                    for(int j=1;j<=4;++j)
                    {
                        is.get();
                        char b = is.peek();
                        if (!is.good())
                            throw BadQuoteString("\\u needs 4 digits");
                        if (b >= '0' && b <='9')
                            data = (data<<4) | (0xF & (b-'0'));
                        else if (b>='a' && b<='f')
                            data = (data<<4) | (0xF & (b-'a'+10));
                        else if (b>='A' && b<='F')
                            data = (data<<4) | (0xF & (b-'A'+10));
                        else BadQuoteString("\\u needs 4 digits");
                    }
                    str += UnicodeToUtf8(data);
                    break;
                }
                default:
                    throw BadQuoteString("unknown char after \\");
            }
            if (byte != 'u') str += byte;
            escaped = false;
        }
        else
        {   // escaped = false here
            if (byte == '\\') escaped = true;
            else if (byte == '"')
            {
                /// " should be escaped as \". An orphan " is not regarded as a part of the string
                /// itself.
                return str;
            }
            else str += byte;
        }
    }
    if (escaped)
    {
        throw BadQuoteString("Missing char after \\");
    }
    return str;
}


/** Split */
std::vector<std::string> SplitString(const std::string& string, const std::string& delim)
{
    return StringToVector<std::string>(string, delim);
}

/**
 * This method's behaviors is not like SplitString(string, string),
 * The difference is below method use the whole delim as a separator,
 * and will scan the target str from begin to end and we drop "".
 * @Return: vector of substring split by delim, without ""
 */
std::vector<std::string> StringSpliter(const std::string& str, const std::string& delim)
{
    std::vector<std::string> v;
    if (str == "")
    {
        return std::vector<std::string>();
    }
    if (delim == "")
    {
        v.push_back(str);
        return v;
    }
    typedef std::string::size_type size_type;
    size_type s_size = str.size();
    size_type d_size = delim.size();
    if (d_size > s_size)
    {
        v.push_back(str);
        return v;
    }
    size_type pos = 0;
    size_type top = s_size - d_size;
    while (pos <= top)
    {
        size_type pos2 = str.find(delim, pos);
        if (pos2 == std::string::npos)
        {
            pos2 = s_size;
        }
        if (pos2 != pos)
        {
            v.push_back(str.substr(pos, pos2 - pos));
        }
        pos = pos2 + d_size;
    }
    if (pos < s_size)
    {
        v.push_back(str.substr(pos));
    }
    return v;
}

/** Trim, LeftTrim and RightTrim */
std::string LeftTrimString(const std::string& string, const char trimChar)
{
    size_t pos = 0;
    while (pos < string.size() && string[pos] == trimChar) ++pos;
    return string.substr(pos);
}

std::string RightTrimString(const std::string& string, const char trimChar)
{
    size_t pos = string.size() - 1;
    while (pos != (size_t)-1  && string[pos] == trimChar) --pos;
    return string.substr(0, pos+1);
}

std::string TrimString(
        const std::string& string,
        const char leftTrimChar,
        const char rightTrimChar)
{
    return LeftTrimString(RightTrimString(string, rightTrimChar), leftTrimChar);
}

std::string ToLowerCaseString(const std::string& orig)
{
    std::string lowerCase(orig);
    std::string::size_type size = lowerCase.size();
    std::string::size_type pos = 0;
    for (; pos < size; ++pos)
    {
        if (isupper(lowerCase[pos]))
        {
            lowerCase[pos] = tolower(lowerCase[pos]);
        }
    }
    return lowerCase;
}

std::string ToUpperCaseString(const std::string& orig)
{
    std::string upperCase(orig);
    std::string::size_type size = upperCase.size();
    std::string::size_type pos = 0;
    for (; pos < size; ++pos)
    {
        if (islower(upperCase[pos]))
        {
            upperCase[pos] = toupper(upperCase[pos]);
        }
    }
    return upperCase;
}

/** Full specialization of ToString for float, double and bool */
template<>
std::string ToString(const float& data)
{
    return ToString(static_cast<double>(data), MAX_FLOAT_PRECISION);
}

template<>
std::string ToString(const double& data)
{
    return ToString(data, MAX_DOUBLE_PRECISION);
}

template<>
std::string ToString(const bool& data)
{
    return data ? "true" : "false";
}

std::string ToString(double data, uint32_t precision)
{
    static const int MAX_SIZE_OF_LITERAL =
            MAX_DOUBLE_PRECISION +
            1 +  // for possible decimal point
            1 +  // for possible char "e"
            5 +  // for expontial after e,
            5;   // BUFFER_FOR_MISTAKE;
    char buf[MAX_SIZE_OF_LITERAL];
    snprintf(buf, MAX_SIZE_OF_LITERAL, "%.*g", precision, data);
    return std::string(buf);
}

/** Full specialization of StringTo for uint8_t, int8_t, and bool */
template<>
uint8_t StringTo(const std::string& string)
{
    uint16_t data = StringTo<uint16_t>(string);
    if (data > 255)
    {
        throw ParameterInvalidException("No valid cast given the string:" + string);
    }
    return static_cast<uint8_t>(data);
}

template<>
int8_t StringTo(const std::string& string)
{
    int16_t data = StringTo<int16_t>(string);
    if (data > 127 || data <-128)
    {
        throw ParameterInvalidException("No valid cast given the string:" + string);
    }
    return static_cast<int8_t>(data);
}

template<>
bool StringTo(const std::string& string)
{
    if (string == TRUE_STRING) return true;
    if (string == FALSE_STRING) return false;
    throw ParameterInvalidException("Invalid string for bool type");
}


std::string operator+(const std::string& left, int32_t data)
{
    return left + ToString(data);
}

std::string operator+(const std::string& left, uint32_t data)
{
    return left + ToString(data);
}

std::string operator+(const std::string& left, int64_t data)
{
    return left + ToString(data);
}

std::string operator+(const std::string& left, uint64_t data)
{
    return left + ToString(data);
}

std::string operator+(const std::string& left, float data)
{
    return left + ToString(data);
}

std::string operator+(const std::string& left, double data)
{
    return left + ToString(data);
}

std::string operator+(const std::string& left, bool data)
{
    return left + ToString(data);
}

std::string EscapeForCCode(const std::string& input)
{
    std::string result;
    for (typeof(input.begin()) it = input.begin();
         it != input.end();
         ++it)
    {
        switch (*it)
        {
            case '"':
                result += "\\\"";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\0':
                result += "\\0";
                break;
            case '\t':
                result += "\\t";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\a':
                result += "\\a";
                break;
            case '\v':
                result += "\\v";
                break;
            case '\f':
                result += "\\f";
                break;
            default:
                result += *it;
        }
    }
    return result;
}

bool StartWith(const std::string& input, const std::string& pattern)
{
    if (input.length() < pattern.length())
    {
        return false;
    }

    size_t i = 0;
    while (i < pattern.length()
        && input[i] == pattern[i])
    {
        i++;
    }

    return i == pattern.length();
};

bool EndWith(const std::string& input, const std::string& pattern)
{
    if (input.length() < pattern.length())
    {
        return false;
    }

    typeof(input.rbegin()) it1 = input.rbegin();
    typeof(pattern.rbegin()) it2 = pattern.rbegin();

    while (it2 != pattern.rend()
        && *it1 == *it2)
    {
        ++it1;
        ++it2;
    }

    return it2 == pattern.rend();
};

 /* string replace */
 std::string ReplaceString(const std::string& origin_string,
         const std::string& old_value,
         const std::string& new_value)
{
     if (old_value.empty())
     {
         SHARELIB_JSON_THROW(ParameterInvalidException, origin_string);
     }

     std::ostringstream s;
     std::string::size_type pos = 0;
     std::string::size_type pos_previous = 0;
     for (;std::string::npos != pos;)
     {
         pos_previous = pos ;
         if ((pos = origin_string.find(old_value, pos)) != std::string::npos)
         {
             if (pos > pos_previous)
             {
                 s << origin_string.substr(pos_previous, pos - pos_previous);
             }
             s << new_value;
             pos += old_value.length() ;
         }
         else
         {
             if (pos_previous + 1 <= origin_string.length())
             {
                s << origin_string.substr(pos_previous);
             }
             break;
         }
     }
     return s.str();
}

std::string FromHexString(const std::string& str)
{
    // if str is converted from StringToHex(string&), then its size must be even.
    if (str.size() % 2)
    {
        SHARELIB_JSON_THROW(ParameterInvalidException,
            "The size of Hex-String must be even.");
    }

    size_t len = str.size() / 2;
    std::string result(len, '\0');
    for (size_t i = 0; i < len; ++i)
    {
        for (int n = 0; n < 2; ++n)
        {
            char c = str[2 * i + n];
            if (c >= 'a' && c <= 'f')
            {
                c -= ('a' - 10);
            }
            else if (c >= 'A' && c <= 'F')
            {
                c -= ('A' - 10);
            }
            else if (c >= '0' && c <= '9')
            {
                c -= '0';
            }
            else
            {
                SHARELIB_JSON_THROW(ParameterInvalidException,
                    "unexpected character in Hex String.");
            }

            result[i] |= c << (4 * (1 - n));
        }
    }
    return result;
}

SHARELIB_ES;
