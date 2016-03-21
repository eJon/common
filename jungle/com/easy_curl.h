//simple using of libcurl
#ifndef __EASY_CURL_H__
#define __EASY_CURL_H__

#include <curl/curl.h>
#include <string>

using namespace std;

namespace com {
struct	curl_opt_t {
    int connect_timeout;
    int timeout;
    int dns_cache_timeout;
    bool keep_alive;
    curl_opt_t() {
        connect_timeout = 100;
        timeout = 100;
        dns_cache_timeout = 100;
        keep_alive = true;
    }
};

class EasyCurl {
  public:
    EasyCurl(): curl_handler_(NULL), curl_list_(NULL) {}
    virtual ~ EasyCurl() {
        curl_easy_cleanup(curl_handler_);
        curl_slist_free_all(curl_list_);
    }
  public:
    CURLcode SetCurlOpt(CURLoption _opt_name, int _opt_value) {
        return	curl_easy_setopt(curl_handler_, _opt_name, _opt_value);
    }
    CURLcode SetCurlOpt(CURLoption _opt_name, const char *_opt_value) {
        return  curl_easy_setopt(curl_handler_, _opt_name, _opt_value);
    }

    virtual int Initialize(const curl_opt_t &_opt);
    virtual int Request(const string &_uri, string &_response_data) ;

    int Append(char *data, size_t size) {
        response_data_.append(data, size);
        return 0;
    }
    static size_t GetResponseData(void *data, size_t size,
                                  size_t nmemb, string &content) {
        long sizes = size * nmemb;
        string temp((char *)data, sizes);
        content += temp;
        return sizes;
    }
    string GetLastError() {
        return curl_err_buffer_;
    }
    
  private:
    CURL *curl_handler_;
    string response_data_;
    struct curl_slist *curl_list_;
    char curl_err_buffer_[CURL_ERROR_SIZE];

};
}//namespace
#endif//__EASY_CURL_H__

