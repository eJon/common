#ifndef MSCOMMON_ICOUNTER_H
#define MSCOMMON_ICOUNTER_H

#include <tr1/memory>
#include <string>
#include <vector>
#include <matchserver/mscom/base/counter.h>

namespace ms {
namespace common{

struct CounterItem {
    std::string adid;
    std::string psid;
    std::string alloc_id;
    std::string adclose;
    std::string show_count;
};

class BrandCounter {
public:
    static const std::string COUNTER_HOST;
    static const std::string COUNTER_CACHE;
//async so not used
    static const uint32_t COUNTER_SAVE_ADID;
    static const uint32_t COUNTER_SAVE_ALLOCID;
public:
    BrandCounter(): counter(NULL) {}
    virtual ~BrandCounter() {
        if(counter != NULL) {
            delete counter;
            counter = NULL;
        }
    }
public:
    virtual int Initialize( const std::string& host,
                            const std::string& max_cache_num);
    int SendRequest(const std::vector<CounterItem> &adsRequest);
private:
    Counter* counter;
};

typedef std::tr1::shared_ptr<BrandCounter> BrandCounterPtr;

}
}/* namespace ms */

#endif //CPMWIRELESS_ADFRONT_COUNTER_H
