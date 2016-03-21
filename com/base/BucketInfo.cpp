#include "BucketInfo.h"
#include "ADutil.h"
#include "strtok.h"
#include "log.h"
#include <iostream>
#include <time.h>
#include <vector>

using namespace std;
using namespace utility;

static const int RET_BUCKET_FILE_ERR = -1;
static const int RET_BUCKET_FILE_EMPTY = -2;

time_t CBucketInfo::m_lastTime = 0;

CBucketInfo::CBucketInfo()
{
}

CBucketInfo::~CBucketInfo()
{
}

bool CBucketInfo::init(const std::string& PIDInfoFile, const std::string& PathIDFile, const std::string& PathInfoFile)
{
	time_t lastModTime = getLastFileModTime(PIDInfoFile.c_str(),PathIDFile.c_str(),PathInfoFile.c_str());
	if(lastModTime > m_lastTime)
		CBucketInfo::m_lastTime = lastModTime;
	else//无文件更新，无需重新加载
		return false;
	
	bool bRet = true;
	//初始化随机数产生器。当查询时，按照随机数获取算法路径。
	srand(time(NULL));
	
	bRet = getConfigData(PIDInfoFile, PathIDFile, PathInfoFile);
	if(bRet == false)
	{
		return false;
	}
	bRet = createMap();
	if(bRet == false)
	{
		return false;
	}
	return true;
}

bool CBucketInfo::getPathInfo(const string& PID,string& pathInfo)
{
	CBucket bucket;
	bool bRet = getPathInfo(PID, bucket);
	if(bRet == false)
	{
		return bRet;
	}
	pathInfo = bucket.toBucketString();
	return true;
}

bool CBucketInfo::getPathInfo(const string& PID, CBucket& bucket)
{
	map<string, vector<ResultPath> >::iterator it;
	
	it = m_mapPidToPath.find(PID);

	vector<ResultPath> vresult;
	//如果PID不在配置数据中，则返回默认路径
	if(it == m_mapPidToPath.end() || PID.empty())
    {
        bucket.setCfgId(0);
        bucket.setBucketId(0);
        bucket.setPathId(0);
        bucket.setAlgo(m_defPathInfo[0].pathInfo);
    }
	else
	{
		vresult = it->second;
		int v;
		v = rand()%100;
		unsigned int i = 0;
		for(; i < vresult.size(); i++)
		{
			if(v < vresult[i].ladder)
			{
				break;
			}
		}
		if(i == vresult.size())
		{
			ERROR(LOGROOT,"不可预期错误，hash结果值不在配置的百分比范围内");
			return false;
		}
		bucket.setCfgId(vresult[i].configId);
		bucket.setBucketId(vresult[i].bucketId);
		bucket.setPathId(vresult[i].bucketId);
		bucket.setAlgo(vresult[i].pathInfo);
	}
	return true;	
}

void CBucketInfo::clear()
{
	m_pidInfo.clear();
	m_pathID.clear();
	m_pathInfo.clear();
	
	m_mapCfgToPath.clear();
	m_mapPidToPath.clear();
}

bool CBucketInfo::getConfigData(const std::string& PIDInfoFile, const std::string& PathIDFile, const std::string& PathInfoFile)
{
	clear();
	
	bool bRet = true;
	//1、加载并解析PID到configID的映射文件
	bRet = load(PIDInfoFile, 0);
	if(bRet == false)
	{
        ERROR(LOGROOT,"load PIDInfoFile false.\n");
		return false;
	}
	
	//2、加载并解析configID到PathID的映射文件
	bRet = load(PathIDFile, 1);
	if(bRet == false)
	{
        ERROR(LOGROOT,"loadPathIDFile false.\n");
		return false;
	}
	bRet = checkPathIDData();
	if(bRet == false)
	{
        ERROR(LOGROOT,"check PathID Data false.\n");
		return false;
	}
	
	//3、加载并解析PathID到PathInfo的映射文件
	bRet = load(PathInfoFile, 2);
	if(bRet == false)
	{
        ERROR(LOGROOT,"loadPathInfoFile false.\n");
		return false;
	}
	bRet = checkPathInfoData();
	if(bRet == false)
	{
        ERROR(LOGROOT,"check PathInfoData false.\n");
		return false;
	}

	bRet = checkValid();
	if(bRet == false)
	{
        ERROR(LOGROOT,"check Valid Data false.\n");
		return false;
	}
	return true;
}

bool CBucketInfo::checkValid()
{
	for(vector<PIDInfo>::iterator it = m_pidInfo.begin(); it != m_pidInfo.end(); it++)
	{
		if(it->configId == -1)
		{
			DEBUG(LOGROOT,"错误：PID配置文件有无效配置！");
			return false;
		}
	}
	for(vector<PathID>::iterator it = m_pathID.begin(); it != m_pathID.end(); it++)
	{
		if(it->configId == -1 || it->percent == -1 || it->bucketId == -1)
		{
			DEBUG(LOGROOT,"错误：PathID配置文件有无效配置！");
			return false;
		}
	}
	return true;
}

bool CBucketInfo::load(const string& dataFile, const int fileType)
{
	bool bRet = true;
	char * pData = NULL, * filePtr = NULL;
	int length = openMMapFile(dataFile);
	//如果文件不存在，或者打开文件失败，报错。
	if(length == RET_BUCKET_FILE_ERR)
	{
		return false;
	}
	//如果算法路径配置文件内容为空，报错。
	else if(length == RET_BUCKET_FILE_EMPTY && fileType == 2)
	{
		return false;
	}
	else if(length > 0)
	{
		filePtr = (char*)m_filePtr;
		pData = new char[length];
		memcpy(pData, filePtr, length);
		bRet = parse(pData, length, fileType);
		if(bRet == false)
		{
			DEBUG(LOGROOT,"错误：解析配置文件%s失败！", dataFile.c_str());
			delete [] pData;
			return false;
		}
		delete [] pData;
	}
	closeMMapFile(length);

	return true;
}

bool CBucketInfo::createMap()
{
	for(vector<PIDInfo>::iterator it = m_pidInfo.begin(); it != m_pidInfo.end(); it++)
	{
		int configId = it->configId;
		map<int, vector<ResultPath> >::iterator itor;
		for(itor = m_mapCfgToPath.begin(); itor != m_mapCfgToPath.end(); itor++)
		{
			if(itor->first == configId)
			{
				break;
			}
		}
		//说明pid对应的configID没有在pathID表里做配置
		if(itor == m_mapCfgToPath.end())
		{
			DEBUG(LOGROOT,"错误：configId=%d不在配置ID到路径ID的映射表中", configId);
			return false;
		}
		vector<ResultPath>& vresult = itor->second;
		for(vector<ResultPath>::iterator its = vresult.begin(); its != vresult.end(); its++)
		{
			int pathId = its->bucketId;
			vector<PathInfo>::iterator itp;
			for(itp = m_pathInfo.begin(); itp != m_pathInfo.end(); itp++)
			{
				if(itp->pathId == pathId)
				{
					break;
				}
			}
			//说明pathID表里存在的pathID没有在PathInfo表里配置
			if(itp == m_pathInfo.end())
			{
				DEBUG(LOGROOT,"错误：pathId=%d不在路径ID到算法路径的映射表中", pathId);
				return false;
			}
			its->pathInfo = itp->pathInfo;
		}
		m_mapPidToPath.insert(pair<string, vector<ResultPath> >(it->pid, vresult));
	}
	return true;
}

bool CBucketInfo::checkPathIDData()
{
	for(vector<PathID>::iterator it = m_pathID.begin(); it != m_pathID.end(); it++)
	{
		map<int, vector<ResultPath> >::iterator itor;
		for(itor = m_mapCfgToPath.begin(); itor != m_mapCfgToPath.end(); itor++)
		{
			if(it->configId == itor->first)
			{
				break;
			}
		}
		if(itor == m_mapCfgToPath.end())
		{
			vector<ResultPath> vresult;
			ResultPath path;
			path.configId = it->configId;
			path.ladder = it->percent;
			path.bucketId = it->bucketId;
			path.pathId = it->bucketId;
			vresult.push_back(path);
			m_mapCfgToPath.insert(pair<int, vector<ResultPath> >(it->configId, vresult));
		}
		else
		{
			ResultPath path;
			path.configId = it->configId;
			path.bucketId = it->bucketId;
			path.pathId = it->bucketId;
			vector<ResultPath>& vpath = itor->second;
			path.ladder = vpath[vpath.size()-1].ladder + it->percent;
			if(path.ladder > 100)
			{
				DEBUG(LOGROOT,"错误：configId=%d的总百分比之和大于100", path.configId);
				return false;
			}
			vpath.push_back(path);
		}
	}

	for(map<int, vector<ResultPath> >::iterator itor = m_mapCfgToPath.begin(); itor != m_mapCfgToPath.end(); itor++)
	{
		vector<ResultPath>& vpath = itor->second;
		if(vpath[vpath.size()-1].ladder < 100)
		{
			ResultPath path;
			path.configId = itor->first;
			path.ladder = 100;
			path.bucketId = 0;
			path.pathId = 0;
			vpath.push_back(path);
		}
	}
	return true;
}		

bool CBucketInfo::checkPathInfoData()
{
	vector<PathInfo>::iterator itor; 
	bool flag = false; //是否提取默认路径标志
	for(itor = m_pathInfo.begin(); itor != m_pathInfo.end(); itor++)
	{
		if(itor->pathId == 0) //ID为0的算法默认路径
		{
			m_defPathInfo[0].pathInfo.modes = (*itor).pathInfo.modes;
			m_defPathInfo[0].pathInfo.rank = (*itor).pathInfo.rank;

			flag = true;
		}
		if(flag)  break;
	}
	if(!flag) return false;

	return true;
}

bool CBucketInfo::parse(char * filePtr, int length, const int fileType)
{
	char * f = filePtr;
	char *line = NULL;
	bool bRet = true;
	while(length >0)
	{
		length = getLine(line, f, length);
		if(*line== '\0')
		{
			continue;
		}
		switch(fileType)
		{
			case 0:
				bRet = parseLineForPID(line);
				break;
			case 1:
				bRet = parseLineForPathID(line);
				break;
			case 2:
				bRet = parseLineForPathInfo(line);
				break;
			default:
				return false;
		}
		if(bRet == false)
		{
			return false;
		}
	}
	return true;
}

bool CBucketInfo::parseLineForPID(char * line)
{
	PIDInfo pidInfo;
	CStrTok strTok(line, " \t", true);
	const char * token = NULL;
	int iLen = 0;
	if((iLen = strTok.nextToken(token)) < 0)
	{
		DEBUG(LOGROOT,"错误：PID配置文件没有第一列");
		cout<<"[error]bucket test: PID config has no first column!"<<endl;
		return false;
	}
	pidInfo.pid = string(token, iLen);
	if((iLen = strTok.nextToken(token)) < 0)
	{
		DEBUG(LOGROOT,"错误：PID配置文件没有第二列");
		cout<<"[error]bucket test: PID config has no second column!"<<endl;
		return false;
	}
	pidInfo.configId = strToNum(token, iLen);
	m_pidInfo.push_back(pidInfo);
	return true;
}

bool CBucketInfo::parseLineForPathID(char * line)
{
	PathID pathID;
	CStrTok strTok(line, " \t", true);
	const char * token = NULL;
	int iLen = 0;
	if((iLen = strTok.nextToken(token)) < 0)
	{
		DEBUG(LOGROOT,"错误：PathID配置文件没有第一列");
		cout<<"[error]bucket test: PathID config has no first column!"<<endl;
		return false;
	}
	if(*token == '-')
	{
		pathID.configId = m_pathID[m_pathID.size() - 1].configId;
	}
	else
	{
		pathID.configId = strToNum(token, iLen);
	}
	if((iLen = strTok.nextToken(token)) < 0)
	{
		pathID.percent = 100;
		pathID.bucketId = 0;
	}
	else
	{
		pathID.percent = strToNum(token, iLen);
		if((iLen = strTok.nextToken(token)) < 0)
		{
			pathID.bucketId = 0;
		}
		else
		{
			pathID.bucketId = strToNum(token, iLen);
		}
	}
	m_pathID.push_back(pathID);
	return true;	
}

char* CBucketInfo::trimStr(char *str)
{
	char * pstart = str;
	char * pcur = str;
	while(*pcur != '\0')
	{
		//跳过空白符
		while(*pcur != '\0' && isspace(*pcur++) != 0);
		//非空白符依次前移
		while(*pcur != '\0' && isspace(*pcur) == 0)
		{ 
			*pstart++ = *pcur++;
		}
	}               
	pstart = '\0'; 
	return str;
}

bool CBucketInfo::parseLineForPathInfo(char * line)
{
	PathInfo thepathInfo;
	CStrTok strTok(line, " \t", true);
	const char * token = NULL;
	int iLen = 0;
	if((iLen = strTok.nextToken(token)) < 0)
	{
		DEBUG(LOGROOT,"错误：PathInfo配置文件没有第一列");
		return false;
	}
	
	thepathInfo.pathId = strToNum(token, iLen);
	if((iLen = strTok.nextToken(token)) < 0)
	{
		DEBUG(LOGROOT,"错误：PathInfo配置文件没有第二列");
		return false;
	}
	string thepath = string(token, iLen);
	vector<string> modList;
	ADutil::Split2(modList, thepath, ';');
    if(MOD_TOTAL != modList.size())
        return false;

    ADutil::Split2(thepathInfo.pathInfo.modes,modList[MOD_MODE].c_str(),',');
    thepathInfo.pathInfo.rank = modList[MOD_RANK]; 
	
	m_pathInfo.push_back(thepathInfo);
	return true;	
}

void CBucketInfo::toUpper(char * str)
{
	if(str == NULL)
	{
		return;
	}
	size_t len = strlen(str);
	for( size_t i = 0; i < len; i++ )
	{
		if ((unsigned char)str[i]>0x80)
		{
			i ++;
		}
		else
		{
			str[i] = toupper(str[i]);
		}
	}
}

int CBucketInfo::openMMapFile(const string& dataFile)
{
    m_fd = ::open(dataFile.c_str(), O_RDONLY, S_IRUSR|S_IWUSR);
	if(m_fd == -1)
	{
		DEBUG(LOGROOT,"错误：打开%s配置文件出错", dataFile.c_str());
		return RET_BUCKET_FILE_ERR;
	}
	struct stat statInfo;
	if(fstat(m_fd, &statInfo) < 0)
	{
		DEBUG(LOGROOT,"错误：获取%s配置文件的属性信息失败", dataFile.c_str());
		::close(m_fd);
		return RET_BUCKET_FILE_ERR;
	}
	int length = statInfo.st_size;
	if(length == 0)
	{
		WARN(LOGROOT,"警告：%s配置文件为空", dataFile.c_str());
		::close(m_fd);
		return RET_BUCKET_FILE_EMPTY;
	}
	m_filePtr = (char*)mmap(0, length, PROT_READ, MAP_SHARED, m_fd, 0);
	if(m_filePtr == MAP_FAILED)
	{
		DEBUG(LOGROOT,"错误：mmap%s配置文件想失败", dataFile.c_str());
		::close(m_fd);
		return RET_BUCKET_FILE_ERR;
	}
	return length;
}

bool CBucketInfo::closeMMapFile(const int length)
{
	munmap(m_filePtr, length);
	::close(m_fd);
	m_fd = 0;
	m_filePtr = NULL;
	return true;
}

int CBucketInfo::getLine(char* &lineStart, char* &filePtr, int length)
{
	//跳过一行的空格
	while(*filePtr == ' ' || *filePtr == '\t')
	{
		filePtr++;
		length--;
	}
	lineStart = filePtr;
	//如果行首为#，跳过该行
	if(*filePtr == '#')
	{
		*filePtr = '\0';
		while(*filePtr != '\n' && *filePtr != '\r')
		{
			filePtr++;length--;
		}
	}
	else
	{
		char * temp = NULL;
		bool bFirst = true;
		//找到行尾（包括#也认为是行尾）
		while(*filePtr != '\n' && *filePtr != '\r' && *filePtr != '#')
		{
			if((*filePtr == ' ' || *filePtr == '\t') && bFirst == true)
			{
				bFirst = false;
				temp = filePtr;
			}
			else if(*filePtr != ' ' && *filePtr != '\t')
			{
				bFirst = true;
				temp = NULL;
			}
				
			filePtr++;
			length--;
		}
		if(temp != NULL)
		{
			*temp = '\0';
		}

		//如果是#结尾，跳过#后该行的字符
		if(*filePtr == '#')
		{
			*filePtr = '\0';
			while(*filePtr != '\n' && *filePtr != '\r')
			{
				filePtr++;length--;
			}
		}
		else
		{
			*filePtr = '\0';
			filePtr++;
			length--;
		}
	}
	//忽略空行
	while(*filePtr == '\n' || *filePtr == '\r')
	{
		filePtr++;
		length--;
	}
	return length;
}

int CBucketInfo::strToNum(const char * str, const int len)
{
	string strTemp(str, len);
	char * ch = (char*)strTemp.c_str();
	while(*ch != '\0')
	{
		if((*ch) >= '0' && (*ch) <= '9')
		{
			ch++;
			continue;
		}
		else
		{
			return -1;
		}
	}

	return atoi(strTemp.c_str());
}

time_t CBucketInfo::getLastFileModTime(const char* PIDInfoFile, const char* PathIDFile, const char* PathInfoFile)
{
	struct stat statInfo1 , statInfo2 ,statInfo3;
        if(stat(PIDInfoFile, &statInfo1) < 0)
        {
                DEBUG(LOGROOT,"错误：获取%s配置文件的属性信息失败", PIDInfoFile);
                return 0;
        }
	if(stat(PathIDFile, &statInfo2) < 0)
        {
                DEBUG(LOGROOT,"错误：获取%s配置文件的属性信息失败", PathIDFile);
                return 0;
        }if(stat(PathInfoFile, &statInfo3) < 0)
        {
                DEBUG(LOGROOT,"错误：获取%s配置文件的属性信息失败", PathInfoFile);
                return 0;
        }
	
	time_t tmp_t = (statInfo1.st_mtime > statInfo2.st_mtime)?statInfo1.st_mtime:statInfo2.st_mtime;
	       tmp_t = (tmp_t > statInfo3.st_mtime)?tmp_t:statInfo3.st_mtime;

	return tmp_t;
}
