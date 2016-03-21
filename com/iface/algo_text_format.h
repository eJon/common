#ifndef CPMCOMMON_ALGO_INTERFACE_ALGO_TEXT_FORMAT_H_
#define CPMCOMMON_ALGO_INTERFACE_ALGO_TEXT_FORMAT_H_
#include <string>
#include <vadecommon/msidx/msidx_plugin_cpm.pb.h>

namespace algo_interface {

class AlgoTextFormat {
  public:
    static void PrintAdlistToString(const ms_dsad::RespAd& adlist, std::string &out);
    //void PrintUserInfoToString(const ms_dsad::RespAd& adlist);
};
}
#endif // CPMCOMMON_ALGO_INTERFACE_ALGO_TEXT_FORMAT_H_
