#include <sharelib/util/long_hash_value.h>
#include <sharelib/util/string_util.h>


SHARELIB_BS;
std::ostream& operator <<(std::ostream& stream, uint128_t v)
{
    char tmp[17] = {0};
    for(int i = 0; i < v.Count(); i++)
    {
        StringUtil::Uint64ToHexStr(v.value[i], tmp, 17);
        stream << tmp;
    }
    return stream;
}

std::ostream& operator <<(std::ostream& stream, uint256_t v)
{
    char tmp[17] = {0};
    for(int i = 0; i < v.Count(); i++)
    {
        StringUtil::Uint64ToHexStr(v.value[i], tmp, 17);
        stream << tmp;
    }
    return stream;
}

std::istream& operator >>(std::istream& stream, uint128_t& v)
{
    char tmp[17] = {0};
    for(int i = 0; i < v.Count(); i++)
    {
        stream.read(tmp, 16);
        StringUtil::HexStrToUint64(tmp, v.value[i]);
    }
    return stream;
}

std::istream& operator >>(std::istream& stream, uint256_t& v)
{
    char tmp[17] = {0};
    for(int i = 0; i < v.Count(); i++)
    {
        stream.read(tmp, 16);
        StringUtil::HexStrToUint64(tmp, v.value[i]);
    }
    return stream;
}

SHARELIB_ES;
