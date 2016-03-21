#ifndef __ISELECT_H_
#define __ISELECT_H_
#include <string>
#include <map>
#include <vector>
#include <mscom/iface/ialgoplugin.h>
#include <mscom/iface/iplugin.h>
#include <vadecommon/msalgo/userinfo.pb.h>

class iselect_t : public iplugin_t {
  public:
    virtual ~iselect_t() {};
  public:
    /**
     * @desc: 解析前端查询串，选择产品插件以及该产品对应的算法插件配置
     * @para:
     *        request[In]         	 前端查询串KeyValue结构
     *        product[Out]	         产品插件
     *        userinfo[Out]      	 用户属性信息
     *        buckets[Out]           算法插件配置
     * @return: 0(SUCCESS) 成功；其他(FAILED) 失败;
     */
    virtual int select(std::map<std::string, std::string> &request,
                       iplugin_t **product,
                       mcuserinfo::UserInfo &userinfo,
                       std::map<std::string, iplugin_t *> &buckets) = 0;
    virtual int check_update() = 0;
    virtual int is_exit() = 0;
};


class IPluginInterface : public  iplugin_t {
  public:
    IPluginInterface() {}
    virtual ~IPluginInterface() {}
  public:
    virtual int Filter(std::map<std::string, std::string> &q_map) = 0;
    /**
     * @desc: 根据前端查询串以及算法插件配置，执行广告业务逻辑，拼装返回结果给客户端
     * @para: q_map          前端查询串KeyValue结构
     *        userinfo       用户属性信息
     *        algo           算法插件配置
     *        adList         返回广告列表
     * @return: 0(SUCCESS) 成功；其他失败；
    */
    virtual int Search(std::map<std::string, std::string> &q_map,
                       mcuserinfo::UserInfo &userinfo,
                       std::map<std::string, iplugin_t *> &algo,
                       std::vector<AdInfo> &adList) = 0;
    /**
     * @desc: 增加用户访问广告的频次
     * @param:  uid           用户id
     *          adlist        广告列表
     * @return: 0(SUCCESS) 成功；其它失败
     */
    virtual int AddFreq(const std::vector<AdInfo> &adList) = 0;
    /**
     * @desc: 拼装返回结果，用于返回给客户端
     * @param:  adList        选中的广告信息
     *          resultString  拼装后的字符串
     * @return: 0(SUCCESS) 成功；其它失败
     */
    virtual int ToString(const std::vector<AdInfo> &adList, std::string &resultString) = 0;
    /**
     * @desc: 拼装错误串，用于返回给客户端
     * @param:
     *          resultString  拼装后的字符串
     * @return: 0(SUCCESS) 成功；其它失败
     */
    virtual int MakeErrorResult(std::string &resultString) = 0;

};

#endif  //__ISELECT_H_


