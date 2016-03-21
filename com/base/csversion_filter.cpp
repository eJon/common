#include <sharelib/util/string_util.h>
#include <matchserver/mscom/base/log.h>
#include <matchserver/mscom/base/csversion_filter.h>

using namespace ms::common;
using namespace sharelib;

CSVersionFilter::CSVersionFilter() {
}

CSVersionFilter::~CSVersionFilter() {
}

int CSVersionFilter::Initialize(const string &_cfg_str, int _tag) {

    //version:platform
    //format:34:1,35:3,33:7
    vector<string> vp;
    string sep = ",";
    vp = StringUtil::Split(_cfg_str, ",");
    size_t size = vp.size();

    for(size_t i = 0; i < size; i++) {
        vector<uint32_t> item;
        sharelib::StringUtil::Split(vp[i], ":", item);

        if(item.size() == 2) {
            min_version_[item[0]] = item[1];
            DEBUG(LOGROOT,"csversion append platform<->version:(%d,%d)", item[0], item[1]);
        }

    }


    return 0;
}

//return value: true--id is in list
//              false--id is not in list
int CSVersionFilter::Check(const string &_id) {
    Version v = Parse(_id);
    DEBUG(LOGROOT,"attachment platform and version (%d,%d)", v.platform, v.version);

    //this is white list attribute
    //if not in configure,filter this !
    if(min_version_.find(v.platform) == min_version_.end()
            || v.version < min_version_[v.platform]) {
        return ISIN_FILTER_LIST;
    }

    return NOTIN_FILTER_LIST;
}
Version CSVersionFilter::Parse(const string &_vstr) {
    //from attachment
    //IsLoadAd;Froms;wm
    //Froms:AABBXXCDDE
    //check:BBC
    int isloadad;
    char froms[50] = {0};
    char wm[50] = {0};
    sscanf(_vstr.c_str(), "%d;%s;%s", &isloadad, froms, wm);
    int version = 0, platform = 0;

    if(strlen(froms) >= strlen("AABBXXCDDE")) {
        version = (froms[2] - '0') * 10 + (froms[3] - '0');
        platform = froms[6] - '0';
    }

    return Version(version, platform);

}

