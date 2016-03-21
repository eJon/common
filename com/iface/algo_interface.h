#ifndef ABTEST_ALGO_INTERFACE_H_
#define ABTEST_ALGO_INTERFACE_H_

/*
 * @Copy right @SINA.COM.CN
 * @author zhangkefeng
 * @time   2013-03-19
 * @email  xidianzkf@gmail.com
 * @weibo  xd_jackfeng
 */
#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include <vadecommon/msalgo/userinfo.pb.h>
#include <vadecommon/msalgo/algo_interface.pb.h>
#include "iplugin.h"


namespace algo_interface {
class AdContext;
class FreqCtlContext;
class FreqCtlContext {
  private:
    FreqContext freq_context_;
  public:
    void SetAdid(const uint64_t &adid);
    uint64_t GetAdid() const;
    void SetFreqency(const uint32_t &freqency);
    uint32_t GetFreqency() const;
    void SetFreqContext(const FreqContext &freq_ctx);
};
//this struct is added for wireless ranking
struct AdElement {
    int id;
    int adid;
    int biztype;
    std::string allocid;
    std::string version;
    std::string ex;
    AdElement() {
        id = 0;
        adid = 0;
        biztype = 0;
        allocid = "0";
        version = "0";
        ex = "";
    }
    AdElement& operator = (const AdElement& _e) {
        id = _e.id;
        adid = _e.adid;
        biztype = _e.biztype;
        allocid = _e.allocid;
        version = _e.version;
        ex = _e.ex;
        return *this;
    }
};
class AbtestAlgoPlugin : public iplugin_t {
  public:
    AbtestAlgoPlugin() {}
    virtual ~AbtestAlgoPlugin() {}

    virtual int Rank2(const std::map<std::string, std::string>& query_map,
		     const mcuserinfo::UserInfo& user_info, void * ad_list,
		     void * ad_ranked) {return 0;}

    virtual int Target2(const std::map<std::string, std::string>& query_map,
		       const mcuserinfo::UserInfo& user_info, void * ad_list){return 0;}

    // depracated
    virtual int Rank(const std::map<std::string, std::string> &query_map,
                     const mcuserinfo::UserInfo &user_info, const AdContext& ad_list,
                     AdContext& ad_result_list) {return 0;}

    // depracated
    //this function is fresh here for wireless ranking
    virtual int Rank(const std::map<std::string, std::string> &_map_query,
                     const mcuserinfo::UserInfo &_user_info, const AdContext &_ad_list,
                     std::vector<AdElement> &_v_ad_result) {
        return 0;
    }
    // depracated
    virtual int Target(const std::map<std::string, std::string> &query_map,
                       const mcuserinfo::UserInfo &user_info, AdContext& ad_list) {return 0;}

    virtual int GetPluginName(std::string& plugin_name) {
        return 0;
    };
};
//
class AdContext {
  private:
    ms_dsad::RespAd  ads_info_;
    AbtestContext abtest_context_;
  public:
    int32_t ChosedFreqIdSize();
    void AddChosedFreqIds(const std::vector<uint64_t>& chosed_freq_ids);
    void GetChosedFreqIds(std::vector<uint64_t>& chosed_freq_ids);
    void AddChosedFreqId(const uint64_t& chosed_freq_id);

    uint64_t GetChosedFreqId(const int& pos);

    void AddFreqContexts(const std::vector<FreqCtlContext>& freq_contexts);
    void AddFreqContext(const FreqCtlContext& freq_context);
    void GetFreqContext(std::vector<FreqCtlContext>& freq_context);
    inline ms_dsad::RespAd* GetMutableAdlist() {
        return abtest_context_.mutable_adlist();
    }
    inline const ms_dsad::RespAd* GetAdlist() const {
        return &abtest_context_.adlist();
    }
};
}

#endif //
