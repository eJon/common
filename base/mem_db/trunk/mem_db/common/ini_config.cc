/**
 * 通用配置文件读取实现
 */
#include <mem_db/common/ini_config.h>

using namespace std;

namespace mem_db_util {
/**
 * 设置区域名称
 * @param areaName 区域名称
 */
void CINIConfig::setAreaName (const char *areaName) {
  if (areaName != NULL) {
    m_areaName = areaName;
    m_areaName_c = m_areaName.c_str ();
  }
}

void CINIConfig::parse (const std::string &profile) {
  m_ini.load (profile.c_str ());
}

int CINIConfig::getInt (const char *key, int def)const {
  return m_ini.get_int (m_areaName_c, key, def);
}

int CINIConfig::getInt (const string &key, int def)const {
  return m_ini.get_int (m_areaName, key, def);
}

unsigned CINIConfig::getUInt (const char *key, unsigned def)const {
  return m_ini.get_unsigned (m_areaName_c, key, def);
}

unsigned CINIConfig::getUInt (const string &key, unsigned def)const {
  return m_ini.get_unsigned (m_areaName, key, def);
}

const char *CINIConfig::getString (const char *key, const char *def)const {
  return m_ini.get_string (m_areaName_c, key, def);
}

const char *CINIConfig::getString (const string &key, const char *def)const {
  static string defStr ("{[NONE]}");
  const string &r = m_ini.get_string (m_areaName, key, defStr);

  if (r == defStr) {
    return def;
  }

  return r.c_str ();
  //return m_ini.get_string(m_areaName_c, key.c_str(), def);
}

bool CINIConfig::getBool (const char *key, bool def)const {
  return m_ini.get_bool (m_areaName_c, key, def);
}

bool CINIConfig::getBool (const std::string &key, bool def)const {
  return m_ini.get_bool (m_areaName, key, def);
}

CINIConfig::CINIConfig (const char *profile) : m_ini (profile), m_areaName (""), m_areaName_c (m_areaName.c_str ()) {
}

CINIConfig::CINIConfig (const std::string &profile) : m_ini (profile), m_areaName (""), m_areaName_c (m_areaName.c_str ()) {
}

CINIConfig::CINIConfig () {
}

CINIConfig::~CINIConfig () {
}

}

