/**
 * 字符串分析。和strtok_r的不同点：1. 每遇到一个分隔符，就返回一个字符串；2. 不修改源串
 */
#ifndef __SHARELIB_STRTOK_H__
#define __SHRAELIB_STRTOK_H__

namespace sharelib
{

class CStrTok
{
public:
	/**
	 * 构造函数
	 * @param str    需要分析的字符串，以'\0'结束
	 * @param sep    分隔符，可以有多个字符，以'\0'结束
	 * @param merge  连续分隔符号是否当作一个处理? 缺省不合成一个处理
	 */
	CStrTok(const char* str, const char* sep, bool merge=false);
	~CStrTok();
	
	/**
	 * 取下一个token
	 * @param token   用于保存下一个token的地址
	 * @return 返回的token的长度。<0表示分析完毕。
	 */
	int NextToken(const char* &token);
private:
	const char* m_str;
	const char* m_sep;
	const char* m_start;
	bool        m_moreToken;
	bool        m_restrict;
};

}

#endif

