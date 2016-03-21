/**
 * 通用配置文件读取
 */
#ifndef __INI_CONFIGURATION_H__
#define __INI_CONFIGURATION_H__

#include <mem_db/common/ini_parser.h>
#include <string>

namespace mem_db_util {

/**
 * 读取配置文件
 */
class CINIConfig {
  public:
    CINIConfig ();
    CINIConfig (const char *profile);
    CINIConfig (const std::string &profile);
    virtual ~CINIConfig ();
    /**
     * 设置区域名称
     * @param areaName 区域名称
     */
    void setAreaName (const char *areaName);

    void parse (const std::string &profile);

    /**
     * 取一个整型值
     * @param key  配置项名称
     * @param def  当指定的配置项不存在时返回的缺省值
     * @return 返回值
     */
    int getInt (const char *key, int def = 0)const;
    int getInt (const std::string &key, int def = 0)const;

    /**
     * 取一个非负整型值
     * @param key  配置项名称
     * @param def  当指定的配置项不存在时返回的缺省值
     * @return 返回值
     */
    unsigned getUInt (const char *key, unsigned def = 0)const;
    unsigned getUInt (const std::string &key, unsigned def = 0)const;

    /**
     * 取一个字符串
     * @param key  配置项名称
     * @param def  当指定的配置项不存在时返回的缺省值
     * @return 返回值
     */
    const char *getString (const char *key, const char *def = NULL)const;
    const char *getString (const std::string &key, const char *def = NULL)const;

    /**
     * 取一个布尔值
     * @param key  配置项名称
     * @param def  当指定的配置项不存在时返回的缺省值
     * @return 返回值
     */
    bool getBool (const char *key, bool def = false)const;
    bool getBool (const std::string &key, bool def = false)const;

  protected:
    INIParser m_ini;

    std::string m_areaName;
    const char *m_areaName_c;
};

}

#endif

