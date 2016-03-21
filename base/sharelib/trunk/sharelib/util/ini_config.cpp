/**
 * 通用配置文件读取实现
 */
#include <sharelib/util/ini_config.h>

using namespace std;

using namespace sharelib;
SHARELIB_BS;
/**
 * 设置区域名称
 * @param areaName 区域名称
 */
void CINIConfig::SetAreaName(const char* areaName)
{
	if (areaName != NULL)
	{
		m_areaName = areaName;
		m_areaName_c = m_areaName.c_str();
	}
}

void CINIConfig::Parse(const std::string& profile)
{
    m_ini.Load(profile.c_str());
}

int CINIConfig::GetInt(const char* key, int def)const
{
	return m_ini.Get_int(m_areaName_c, key, def);
}

int CINIConfig::GetInt(const string& key, int def)const
{
	return m_ini.Get_int(m_areaName, key, def);
}

unsigned CINIConfig::GetUInt(const char* key, unsigned def)const
{
	return m_ini.Get_unsigned(m_areaName_c, key, def);
}

unsigned CINIConfig::GetUInt(const string& key, unsigned def)const
{
	return m_ini.Get_unsigned(m_areaName, key, def);
}

const char* CINIConfig::GetString(const char* key, const char* def)const
{
	return m_ini.Get_string(m_areaName_c, key, def);
}

const char* CINIConfig::GetString(const string& key, const char* def)const
{
	static string defStr("{[NONE]}");
	const string &r = m_ini.Get_string(m_areaName, key, defStr);
	if (r == defStr)
		return def;
	return r.c_str();
	//return m_ini.Get_string(m_areaName_c, key.c_str(), def);
}

bool CINIConfig::GetBool(const char* key, bool def)const
{
	return m_ini.Get_bool(m_areaName_c, key, def);
}

bool CINIConfig::GetBool(const std::string& key, bool def)const
{
	return m_ini.Get_bool(m_areaName, key, def);
}

CINIConfig::CINIConfig(const char *profile) : m_ini(profile),m_areaName(""),m_areaName_c(m_areaName.c_str())
{
}

CINIConfig::CINIConfig(const std::string& profile) : m_ini(profile),m_areaName(""),m_areaName_c(m_areaName.c_str())
{
}

CINIConfig::CINIConfig()
{
}

CINIConfig::~CINIConfig()
{
}

SHARELIB_ES;

