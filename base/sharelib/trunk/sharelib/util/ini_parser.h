/**
 * INI读取工具
 *
 * INI文件格式:
 * [section]
 * # 注释行
 * key1 = value1 # 可以有行尾注释。可用通过get_string/get_int等函数获取key的值
 * key2 = value2 # 其它key
 * ...           # 整个section的非注释行可用通过get_section获得
 *
 * [其它section]
 * ...
 */
#ifndef _SHARELIB_INI_PARSER_H_
#define _SHARELIB_INI_PARSER_H_

#include <string>
#include <map>
#include <string.h>
#include <stdlib.h>

namespace sharelib
{

typedef std::map<std::string, std::string> stringMap; // 保存key => value
typedef std::map<std::string, stringMap> sectionMap;  // 保存section => key/value

class INIParser
{
public:
	/**
	 * 构造函数
	 * @param profile  INI文件名
	 */
	INIParser();
	INIParser(const char *profile);
	INIParser(const std::string &profile);
	
	virtual ~INIParser();
	
	/**
	 * 加载配置文件(同时清空原有的配置信息)
	 * @param profile  INI文件名
	 */
	void Load(const char *profile);
	void Load(const std::string &profile);
	
	/**
	 * 获取一个字符串
	 * @param section  section名
	 * @param key      变量名
	 * @param def      当变量不存在时的缺省值
	 * @return 指定变量的值
	 */
	const std::string &Get_string(const std::string &section, const std::string &key, const std::string &def)const;
	const char *Get_string(const char *section, const char *key, const char *def = NULL)const;
	
	/**
	 * 获取一个整型值
	 * @param section  section名
	 * @param key      变量名
	 * @param def      当变量不存在时的缺省值
	 * @return 指定变量的值
	 */
	int Get_int(const std::string &section, const std::string &key, int def = 0)const;
	int Get_int(const char *section, const char *key, int def = 0)const;
	
	/**
	 * 获取一个非负整型值
	 * @param section  section名
	 * @param key      变量名
	 * @param def      当变量不存在时的缺省值
	 * @return 指定变量的值
	 */
	unsigned Get_unsigned(const std::string &section, const std::string &key, unsigned def = 0)const;
	unsigned Get_unsigned(const char *section, const char *key, unsigned def = 0)const;
	
	/**
	 * 获取一个布尔值。布尔值的字符串为"Y"/"YES"/"T"/"True"都为真，其余为假
	 * @param section  section名
	 * @param key      变量名
	 * @param def      当变量不存在时的缺省值
	 * @return 指定变量的值
	 */
	bool Get_bool(const std::string &section, const std::string &key, bool def = false)const;
	bool Get_bool(const char *section, const char *key, bool def = false)const;
	
	/**
	 * 获取某一个section内容
	 * @param section   指定的section名
	 * @return 相应的内容
	 */
	std::string &Get_section(const std::string &section);
	const char *Get_section(const char *section);
	
	/**
	 * 导出ini的内容，测试用
	 */
	void Dump()const;
private:
	sectionMap m_ini;
	stringMap m_sections;
	std::string m_none;
	std::string NOTHING;
	
	bool Find_section(const char *p, std::string &section, std::string &sq_section, std::string &sec_body);
	const char *Find_pair(const char *p, std::string &section);
	void Append_sq_section(std::string &sq_section, std::string &sec_body, const char *p, int len=-1);
};

}

#endif

