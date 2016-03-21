#ifndef SHARELIB_BASE64_H_
#define SHARELIB_BASE64_H_
#include <sharelib/common.h>
#include <string>
SHARELIB_BS

class Base64 {
public:

    /**
     * @brief Base64编码
     *
     * @param src 待编码序列
     * @param line_len 换行字节数, 必须为4的倍数，<=0则不换行
     *
     * @return 编码序列
     */
    static std::string encode(const std::string& src, int line_len=76);

    /**
     * @brief Base64解码
     *        调用前应先判断输入序列是否为Base64合法编码，否则结果不可预期
     *
     * @param src 待解码序列
     *
     * @return 解码序列
     */
    static std::string decode(const std::string& src);

    /**
     * @brief 判断是否Base64合法编码
     *
     * @param src 编码序列
     *
     * @return true/false
     */
    static bool isValid(const std::string& src);

private:
    // 编码表
    static const char m_encode_table[];

    // 解码表
    static const char m_decode_table[];
};

SHARELIB_ES
#endif //SHARELIB_BASE64_H_

