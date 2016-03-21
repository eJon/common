#include "easy_curl.h"

namespace com {
int EasyCurl::Initialize(const curl_opt_t &_opt) {
    CURLcode res;
    curl_handler_ = curl_easy_init();

    if (_opt.keep_alive) {
	curl_list_ = curl_slist_append(curl_list_, "Connection: keep-alive");
    }

    res = curl_easy_setopt(curl_handler_, CURLOPT_HTTPHEADER, curl_list_);
    //all default configure,if you wanna change the default configure,
    //please invoke SetCurlOpt function directly
    res = SetCurlOpt(CURLOPT_NOSIGNAL, 1);
    res = SetCurlOpt(CURLOPT_CONNECTTIMEOUT_MS, _opt.connect_timeout);
    res = SetCurlOpt(CURLOPT_TIMEOUT_MS, _opt.timeout);
    res = SetCurlOpt(CURLOPT_DNS_CACHE_TIMEOUT, _opt.dns_cache_timeout);
    res = curl_easy_setopt(curl_handler_, CURLOPT_ERRORBUFFER, curl_err_buffer_);
    res = curl_easy_setopt(curl_handler_, CURLOPT_WRITEFUNCTION, GetResponseData);
    res = curl_easy_setopt(curl_handler_, CURLOPT_WRITEDATA, &response_data_);
    //curl_easy_setopt(curl_handler_, CURLOPT_MAXCONNECTS , 100L);
    return 0;

}
int EasyCurl::Request(const string &_uri, string &_response_data) {
    CURLcode res;
    response_data_ = "";
    res = curl_easy_setopt(curl_handler_, CURLOPT_URL, _uri.c_str());
    res = curl_easy_perform(curl_handler_);

    if(CURLE_OK != res) {
        return 1;
    }

    _response_data = response_data_;
    return 0;
}

}
