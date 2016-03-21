#ifndef  __IALGOPLUGIN_TREND_H__
#define  __IALGOPLUGIN_TREND_H__

#include <inttypes.h>
#include <vector>
#include <map>
#include <string>
#include <mscom/iface/iplugin.h>


struct AdSeed {
    uint32_t native_bhv_type;                   // 原生广告时候, 互动类型                                                                                 
    uint32_t native_bhv_time;                   // 原生广告时候, 互动时间                                                                                 
    uint32_t native_bhv_uid;                    // 原生广告时候, 互动uid
    uint64_t native_post_objid;                 // 原生广告时候, 展示feedid
    uint64_t native_raw_objid;                  // 原生广告时候, 广告主feedid (一般没有用)
};
struct AdInfo {
    uint64_t	custid_;			//广告主Id
    uint64_t	adid_;				//广告Id
    uint64_t	feedid_;			//FeedId
    uint64_t	feed_ctime_;			//feed发布时间
    uint32_t	price_;				//广告主的出价价格
    uint32_t	bidtype_;			//1：CPF 2：CPE
    uint32_t	matchtype_;			//0:fans 1:non-fans 2:all
    uint32_t	target_score_;			//0-1000
    double		ctr_;				//
    float		target_ctr_;			//target模块预估的ctr值
    double		rank_score_;			//
    uint32_t 	cost_;				//实际扣费价格
    uint32_t	pos_;				//广告曝光位置
    std::string	hostname_;			//返回该Feed的索引服务机器名
    std::string	creative_;			//广告创意,json格式

    std::map<std::string, uint64_t> match_result_;	//索引匹配结果

    std::vector<AdSeed> seeds;                  // 原生信息
    int selected_seed;
    int isfilter;
};

class DefaultAlgoPlugin : public iplugin_t {
 public:
    DefaultAlgoPlugin() {}
    virtual ~DefaultAlgoPlugin() {}
    /**
     * @desc: 根据前端查询串，返回TopN广告列表以及用户信息
     * @param:  q_map        前端查询串KeyValue结构
     *          iTopN        请求返回TopN的广告列表
     *          adInfoList   获取TopN的广告列表
     *          userinfo     用户信息Key/Value串
     * @return 0(SUCCESS) 成功；其他失败;
     */
    virtual int target(const std::map<std::string, std::string> &q_map,
                       int  iTopN,
                       std::vector<AdInfo> &adInfoList,
                       std::map<std::string, std::string> &userinfo) {
        return 0;
    };



    /**
     * @desc: 根据前端查询串和用户信息，对传入的广告列表进行排序
     * @param:  q_map        前端查询串KeyValue结构
     *          userinfo     用户信息Key/Value串
     *          adInfoList   排序后的广告列表
     *          ad_choose    Rank选择的广告位置
     * @return 0(SUCCESS) 成功；其他失败;
     */
    virtual int rank(const std::map<std::string, std::string> &q_map,
                     const std::map<std::string, std::string> &userinfo,
                     std::vector<AdInfo> &adInfoList, int &ad_choose,
                     std::map<std::string, std::string> &feature_map) {
        return 0;
    };

    virtual int rank2(const std::map<std::string, std::string> &q_map,
		      const std::map<std::string, std::string> &userinfo,
		      std::vector<AdInfo> & adInfoList,
		      std::map<std::string, std::string> &feature_map) {
        return 0;
    };
    /**
     * @desc: 获取插件名称
     * @param:  name         插件名称
     * @return 1(SUCCESS) 成功；其他失败;
     */
    virtual int getPluginName(std::string &name) = 0;
};


#endif //__IALGOPLUGIN_TREND_H__


