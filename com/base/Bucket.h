/***************************************************************************************
 * bucket test的一条测试路径管理类。
 * 可以通过该类根据模块名称获取在该测试路径上应该使用该模块的版本号。
 *
 * *************************************************************************************/

#ifndef __BUCKET_H__
#define __BUCKET_H__

#include <string>
#include <vector>

#define DEF_PATHID   0		//默认算法路径

using namespace std;

//模块的序号，有先后顺序关系
enum ModuleName
{
	MOD_MODE = 0,
	MOD_RANK = 1,
	MOD_TOTAL= 2
};

//算法路径，整形数组按照顺序存储各模块使用的版本号
typedef struct AlgoPath
{
    std::string rank;
    std::vector<std::string> modes;
}AlgoPath;

class CBucket
{
public:
	CBucket();
	~CBucket();

	/**获取算法路径的字串格式
	 * param 
	 * return：返回算法的字串格式
	 */
	std::string toBucketString()const;

	/**获取配置ID
	 */
	int getCfgId()const { return m_configId; }
	/**获取bucketID
	 */
	int getBucketId()const { return m_bucketId; }
	/**判断是否为默认bucket
         */
	bool isDefAlgoPath()const { return (m_pathId == DEF_PATHID)?true:false; }
	/**获取算法路径ID
	 */
	int getPathId()const { return m_pathId; }
	
	/**设置配置ID
	 */
	void setCfgId(const int configId)
	{
		m_configId = configId;
	}
	/**设置bucketID
	 */
	void setBucketId(const int bucketId)
	{
		m_bucketId = bucketId;
    }
    
    void setAlgo(const AlgoPath& algoPath)
    {
        m_path.modes = algoPath.modes;
        m_path.rank = algoPath.rank; 
    }

    void getAlgo(std::vector<std::string>&  modes, std::string& rank)
    {
        modes = m_path.modes;
        rank = m_path.rank; 
    }

	/**设置算法路径ID
	 */
	void setPathId(const int pathId)
	{
		m_pathId = pathId;
	}

private:
	int m_configId;
	int m_bucketId;
	int m_pathId;
	AlgoPath m_path;
};

#endif
