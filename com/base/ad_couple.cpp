// copyright:
//    (C) Sina Weibo Inc.
//
//      file: ad_couple.cc
//      desc:
//    author: tieniu
//     email: tieniu@staff.sina.com.cn
//      date: 2014-03-25
//
//    change:

#include <matchserver/mscom/base/ad_couple.h>
#include <matchserver/mscom/base/log.h>
#include <matchserver/mscom/base/wstring_util.h>
#include <sharelib/util/string_util.h>

using namespace sharelib;
using namespace std;   // to avoid bug for md5util
#include <matchserver/mscom/base/md5util.h>

namespace ms {
namespace common{

AdCouple::AdCouple() : is_working_(0),
    max_time_for_message_dwelling_(0),
    max_dwelling_message_items_(0),
    master_ad_couple_meta_queue_ptr_(NULL),
    slave_ad_couple_meta_queue_ptr_(NULL),
    curl_handler_(NULL) {
}

AdCouple::~AdCouple() {
    Release();
}

int AdCouple::Initialize(int max_dwelling_message_items, int max_time_for_message_dwelling) {

    if (max_time_for_message_dwelling <= 0) {
        max_time_for_message_dwelling_ = DEFAULT_MAX_TIME_FOR_MESSAGE_DWELLING;
    } else {
        max_time_for_message_dwelling_ = max_time_for_message_dwelling;
    }

    if (max_dwelling_message_items <= 0) {
        max_dwelling_message_items_ = DEFAULT_MAX_DWELLING_MESSAGE_ITEMS;
    } else {
        max_dwelling_message_items_ = max_dwelling_message_items;
    }

    if(NULL != master_ad_couple_meta_queue_ptr_) {
        delete master_ad_couple_meta_queue_ptr_;
        master_ad_couple_meta_queue_ptr_ = NULL;
    }
    master_ad_couple_meta_queue_ptr_ = new std::queue<AdCoupleMeta>();
    if (NULL == master_ad_couple_meta_queue_ptr_) goto err;

    if(NULL != slave_ad_couple_meta_queue_ptr_) {
        delete slave_ad_couple_meta_queue_ptr_;
        slave_ad_couple_meta_queue_ptr_ = NULL;
    }
    slave_ad_couple_meta_queue_ptr_ = new std::queue<AdCoupleMeta>();
    if (NULL == slave_ad_couple_meta_queue_ptr_) goto err;

    curl_handler_ = curl_easy_init();
    if (NULL == curl_handler_) goto err;

    // example.com is redirected, so we tell libcurl to follow redirection
    curl_easy_setopt(curl_handler_, CURLOPT_FOLLOWLOCATION, 1L);

    // begin to work now
    is_working_ = 1;

    Run();  // function inherited from base class
    return 0;

err:
    if(NULL != master_ad_couple_meta_queue_ptr_) {
        delete master_ad_couple_meta_queue_ptr_;
        master_ad_couple_meta_queue_ptr_ = NULL;
    }
    if(NULL != slave_ad_couple_meta_queue_ptr_) {
        delete slave_ad_couple_meta_queue_ptr_;
        slave_ad_couple_meta_queue_ptr_ = NULL;
    }

    return 1;
}

int AdCouple::Release() {
    // terminate working
    is_working_ = 0;
    Stop();  // function inherited from base class

    if(NULL != master_ad_couple_meta_queue_ptr_) {
        delete master_ad_couple_meta_queue_ptr_;
        master_ad_couple_meta_queue_ptr_ = NULL;
    }
    if(NULL != slave_ad_couple_meta_queue_ptr_) {
        delete slave_ad_couple_meta_queue_ptr_;
        slave_ad_couple_meta_queue_ptr_ = NULL;
    }

    // always cleanup
    curl_easy_cleanup(curl_handler_);

    return 0;
}


int AdCouple::AsyncSendAdCoupleData(const AdCoupleMeta &ad_couple_meta) {
    if (0 == is_working_) {
        // is not working
	DEBUG(LOGROOT,"third party server is not working");
        return 0;
    }

    master_ad_couple_meta_queue_ptr_->push(ad_couple_meta);

    return 0;
}

void* AdCouple::InternalProcess() {
    AdCoupleMetaQueuePtr temp;
    AdCoupleMeta ad_couple_meta;

    while (onRunning) {
        for (int i = 0; i < max_time_for_message_dwelling_; ++i) {
            ::sleep(1);

            if (master_ad_couple_meta_queue_ptr_->size() >= (size_t)max_dwelling_message_items_) {
                break;
            }
        }

        // switch master queue  with slave queue;
        temp = master_ad_couple_meta_queue_ptr_;
        master_ad_couple_meta_queue_ptr_ = slave_ad_couple_meta_queue_ptr_;
        slave_ad_couple_meta_queue_ptr_ = temp;

        // send out ad couple meta data
        while(!slave_ad_couple_meta_queue_ptr_->empty()) {
            ad_couple_meta = slave_ad_couple_meta_queue_ptr_->front();
            slave_ad_couple_meta_queue_ptr_->pop();

            SendOutAdCoupleMetaData(ad_couple_meta);
        }
    }

    return NULL;
}

int AdCouple::SendOutAdCoupleMetaData(const AdCoupleMeta &ad_couple_meta) {
    StringUtil su;
    string url_prefix = ad_couple_meta.url_prefix;
    if (url_prefix == "") {
        WARN(LOGROOT,"third party curl failed of null url prefix");
        return -1;
    }
    string uid_code = Md5Util::Encode(ad_couple_meta.uid);
    su.ReplaceAll(url_prefix, "{wb_uid_md5}", uid_code);
    char time_str[50] = {0};
    snprintf(time_str,50,"%lu",time(NULL));
    su.ReplaceAll(url_prefix, "{timestamp}", time_str);
    //url += ad_couple_meta.url_prefix + "&uid=" + uid_code + "&adid=" + ad_couple_meta.adid;

    curl_easy_setopt(curl_handler_, CURLOPT_URL, url_prefix.c_str());
    CURLcode ret;
    ret = curl_easy_perform(curl_handler_);
    if (ret != CURLE_OK) {
	WARN(LOGROOT,"third party curl failed,return code:%d,url:%s",
			ret,url_prefix.c_str());
        return -1;
    }
    DEBUG(LOGROOT,"third party curl ok,return code:%d,url:%s",
			ret,url_prefix.c_str());

    return 0;
}

}
}/* namespace ms */
