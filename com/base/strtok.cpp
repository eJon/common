/**
 * 字符串分析。和strtok_r的不同点：1. 每遇到一个分隔符，就返回一个字符串；2. 不修改源串
 */
#include "strtok.h"
#include <string.h>

namespace utility
{

/**
 * 构造函数
 * @param str    需要分析的字符串，以'\0'结束
 * @param sep    分隔符，可以有多个字符，以'\0'结束
 * @param merge  连续分隔符号是否当作一个处理? 缺省不合成一个处理
 */
CStrTok::CStrTok(const char* str, const char* sep, bool merge) : 
	m_str(str), m_sep(sep), m_start(str), m_restrict(merge)
{
	m_moreToken = (str != NULL && str[0] != '\0' && sep != NULL && sep[0] != '\0');
	
	if (m_moreToken && m_restrict)
	{
		while ('\0' != *m_start && strchr(m_sep, *m_start) != NULL) ++m_start; // 滤掉所有开头的分隔符
		if (*m_start == '\0') m_moreToken = false;
	}
}

CStrTok::~CStrTok()
{
}

/**
 * 取下一个token
 * @param token   用于保存下一个token的地址
 * @return 返回的token的长度。<0表示分析完毕。
 */
int CStrTok::nextToken(const char* &token)
{
	if (!m_moreToken)
	{
		token = NULL;
		return -1;
	}
	
	token = m_start;  // 记录token起始地址
	while ('\0' != *m_start && strchr(m_sep, *m_start) == NULL) ++m_start; // 寻找分隔符
	
	int len = m_start - token; // 计算token的长度 
	if (*m_start == '\0')
		m_moreToken = false;
	else
	{
		m_start++;             // 跳过一个分隔符
		if (m_restrict)
		{
			// 多个分隔符合为一个
			while ('\0' != *m_start && strchr(m_sep, *m_start) != NULL) ++m_start; // 跳过连续的分隔符
			if (*m_start == '\0') m_moreToken = false;
		}
	}
	
	return len;
}

}

