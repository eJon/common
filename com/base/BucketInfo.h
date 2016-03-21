/********************************************************************************************************
 * bucket test功能类。
 * 该类通过三个配置文件信息：PID到CongfigID、ConfigID到PathID、PathID到PathInfo
 * 根据PID和cookie信息确定一条PathInfo路径。之后的各模块根据该路径信息确定执行哪个版本。
 *
 * *****************************************************************************************************/

#ifndef __BUCKETINFO_H__
#define __BUCKETINFO_H__

#include <string>
#include <vector>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "Bucket.h"

#define	ALGO_DEFPATH_NUM 1  

typedef struct PIDInfo
{
	std::string pid;	//主键，唯一。
	int configId;		//配置ID
}PIDInfo;

typedef struct PathID
{
	int configId;		//主键，可重复。
	int percent;		//算法路径的流量
	int bucketId;		//buckeId
}PathID;

typedef struct PathInfo
{
	int pathId;		//主键，唯一
	AlgoPath pathInfo;	//算法路径
}PathInfo;

typedef struct ResultPath
{
	int configId;
	int ladder;		//该configId对应的梯段（由percent获得）
	int bucketId;
	int pathId;
	AlgoPath pathInfo;
}ResultPath;

class CBucketInfo
{
public:
	CBucketInfo();
	~CBucketInfo();
	
	/**加载并解析三个配置文件。根据三个配置文件的关联情况，生成m_mapPidToPath结构，用于最终的查询。
	 * param PIDInfoFile：	[in]PID和配置ID映射表文件。
	 * param PathIDFile：	[in]配置和算法路径映射表文件。
	 * param PathInfoFile：	[in]算法路径配置文件。
	 * return：成功返回true，失败false。
	 */
	bool init(const std::string& PIDInfoFile, const std::string& PathIDFile, const std::string& PathInfoFile);

	/**根据PID和cookie信息，获取算法路径。
	 * param PID：		[in]query传入的合作伙伴ID
	 * param cookie：	[in]query传入的cookie信息
	 * param pathInfo：	[inout]返回算法路径的字串形式
	 * param bucket：	[inout]返回算法路径的结构形式
	 * return：成功返回true，失败false。（查不到也属于失败）
	 */
	bool getPathInfo(const std::string& PID, std::string& pathInfo);
	bool getPathInfo(const std::string& PID, CBucket& bucket);

private:
	
#ifdef TEST_FOR_BUCKETINFO	//测试时使用
public:
	std::vector<PIDInfo>& getPIDInfo()	{ return m_pidInfo; }
	std::vector<PathID>& getPathID()		{ return m_pathID; }
	std::vector<PathInfo>& getPathInfo()	{ return m_pathInfo; }
	
	std::map<int, std::vector<ResultPath> > & getMapCfgToPath()	{ return m_mapCfgToPath; }
	std::map<std::string, std::vector<ResultPath> >& getMapPidToPath() { return m_mapPidToPath; }
public:
#endif

	//加载并解析三个配置文件，生成原始的三个vector结构。同时，对数据做校验和容错补充。
	bool getConfigData(const std::string& PIDInfoFile, const std::string& PathIDFile, const std::string& PathInfoFile);
	//根据原始的三个vector结构，创建查询使用的map结构。该结构直接从PID对应到它所属的多个path路径结构。
	bool createMap();
	//清空所有结构内容	
	void clear();

	//对三个文件分别加载和解析，并对相应的结构做调整。按照fileType判断处理哪个文件
	bool load(const std::string& dataFile, const int fileType);

	//解析某个文件，按照fileType判断处理哪个文件。
	bool parse(char * filePtr, int length, const int fileType);
	//解析PID文件的一行
	bool parseLineForPID(char * line);
	//解析PathID文件的一行。对'-'做处理
	bool parseLineForPathID(char * line);
	//解析PathInfo文件的一行
	bool parseLineForPathInfo(char * line);

	//检查PathID文件解析结果的正确性，生成configID到其对应的Path集合的映射结构m_mapCfgToPath，这里该结构的PathInfo为无效值。
	bool checkPathIDData();
	//检查PathInfo文件解析结果的正确性，并对空缺处项按照pathID=0的默认路径做补充。
	bool checkPathInfoData();
	//检查配置文件是否正确（主要是配置项为数字的是否含有非数字字符）
	bool checkValid();

	//根据模块名获取模块的位置序号
	int getModIndex(const std::string& modName);
	void toUpper(char * str);	

	char* trimStr(char *str);

	//MMap数据文件
	int openMMapFile(const std::string& dataFile);
	//关闭数据文件，释放资源
	bool closeMMapFile(const int length);

	//截取一行数据
	int getLine(char* & lineStart, char* &filePtr, int length);

	//将字符串转化为数字，如果该字符串里含有非数字，返回-1。转化成功返回相应数字。
	int strToNum(const char * str, const int len);
	
	//获取最新文件修改时间
	time_t getLastFileModTime(const char* PIDInfoFile, const char* PathIDFile, const char* PathInfoFile);
private:
	std::vector<PIDInfo> m_pidInfo;		//解析PID文件的结果
	std::vector<PathID> m_pathID;		//解析PathID文件的结果
	std::vector<PathInfo> m_pathInfo;	//解析PathInfo文件的结果
	
	PathInfo m_defPathInfo[ALGO_DEFPATH_NUM];	//默认算法路径;0是搜索,1是非搜索
	
	//中间结果，configID与多个Path的映射
	std::map<int, std::vector<ResultPath> > m_mapCfgToPath;
	//使用该结构查询，先根据pid查到对应的pathInfo集合，再根据cookie从集合中选取一个。
	std::map<std::string, std::vector<ResultPath> > m_mapPidToPath;

	void * m_filePtr;			//MMap文件的内存指针，临时使用
	int m_fd;				//打开文件的句柄，临时使用

	static time_t m_lastTime; //保存bucketInfo配置文件最新更新时间
};

#endif
