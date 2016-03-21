// copyright:
//    (C) Sina Weibo Inc.
//
//      file: ad_couple.h
//      desc:
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date: 2014-03-25
//
//    change:

#ifndef BRAND_MATCHSERVER_BRAND_AD_COUPLE_H_
#define BRAND_MATCHSERVER_BRAND_AD_COUPLE_H_

#include <sharelib/util/thread_base.h>
#include <curl/curl.h>
#include <string>
#include <queue>


namespace ms {
namespace common{

struct AdCoupleMeta {
    std::string uid;
    std::string adid;
    std::string url_prefix;
};

#define DEFAULT_MAX_TIME_FOR_MESSAGE_DWELLING 5  // in seconds
#define DEFAULT_MAX_DWELLING_MESSAGE_ITEMS 100


typedef std::queue<AdCoupleMeta>* AdCoupleMetaQueuePtr;

class AdCouple : CThreadBase {
public:
    AdCouple();
    ~AdCouple();

    int Initialize(int max_dwelling_message_items = DEFAULT_MAX_DWELLING_MESSAGE_ITEMS,
                   int max_time_for_message_dwelling = DEFAULT_MAX_TIME_FOR_MESSAGE_DWELLING);
    int Release();
    int AsyncSendAdCoupleData(const AdCoupleMeta &ad_couple_meta);

private:
    int SendOutAdCoupleMetaData(const AdCoupleMeta &ad_couple_meta);
    virtual void* InternalProcess();

    int is_working_;

    int max_time_for_message_dwelling_;
    int max_dwelling_message_items_;

    AdCoupleMetaQueuePtr master_ad_couple_meta_queue_ptr_;
    AdCoupleMetaQueuePtr slave_ad_couple_meta_queue_ptr_;

    CURL *curl_handler_;

    // no assign and no copy
    AdCouple(const AdCouple&);
    AdCouple &operator = (const AdCouple);
};

}
}/* namespace ms */

#endif  // BRAND_MATCHSERVER_BRAND_AD_COUPLE_H_
