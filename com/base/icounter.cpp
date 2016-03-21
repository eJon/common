#include <sharelib/util/time_utility.h>
#include <sharelib/util/string_util.h>
#include <matchserver/mscom/base/icounter.h>
#include <matchserver/mscom/base/log.h>

using namespace std;
namespace ms {
namespace common{

const std::string BrandCounter::COUNTER_HOST = "counter_host";
const std::string BrandCounter::COUNTER_CACHE = "counter_cache";
const uint32_t BrandCounter::COUNTER_SAVE_ADID = 1000;
const uint32_t BrandCounter::COUNTER_SAVE_ALLOCID = 100;

int BrandCounter::Initialize(const std::string& host, const string& max_cache_num) {
    int ret;
    counter = new Counter();
    int cache;
    if( sscanf( max_cache_num.c_str(), "%d", &cache ) == EOF ) {
        return 1;
    }

    if( 0!= (ret = counter->Init(host, cache))) {
        return 2;
    }
    return 0;
}

int BrandCounter::SendRequest(const vector<CounterItem> &adsRequest)
{
    string date = TimeUtility::CurrentTimeDateReadable();
    string showCount;
    int size_item = adsRequest.size();
    for(uint32_t i =0; i < size_item; i++) {
        //allocid
        DEBUG(LOGROOT, "[BrandCounter::SendRequest]adid-psid:" + adsRequest[i].adid + date + "-" + adsRequest[i].psid);
        if(adsRequest[i].alloc_id != "" && adsRequest[i].alloc_id != "0") {
            if(0 != counter->Send(adsRequest[i].alloc_id + date,
				    adsRequest[i].show_count, adsRequest[i].psid)) {
                ERROR(LOGROOT, "send alloc counter error");
            }
        }
        //adid
        if(0 != counter->Send(adsRequest[i].adid + date,
			       	adsRequest[i].show_count, adsRequest[i].psid)) {
            ERROR(LOGROOT, "send adid counter error");
        }
    }

    return 0;
}

}
}/* namespace ms */

