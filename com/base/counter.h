#ifndef MSCOMMON_COUNTER_H
#define MSCOMMON_COUNTER_H


#include <stdlib.h>
#include <string>

#include <tr1/unordered_map>

#include <curl/curl.h>


namespace ms {
namespace common{

class Counter {
public:
    Counter();
    Counter(const std::string &host, int cache_value);
    ~Counter();

    int Init(const std::string &host, int cache_value);
    inline void SetHost(const std::string &host) {
        m_host = host;
    }

    int Send( const std::string& key, const std::string&  value, const std::string& pdps);

private:
    void Clean();
    std::string GenerateKey(const std::string &adid, const std::string &pdps);
    bool ParseKey(const std::string &key, std::string &adid, std::string &pdps);
    int Insert2Cache(const std::string &adid, const std::string &value, const std::string &pdps);
    int SendDataToRemote(const std::string &key, int val);
    void FlushAll();

    CURL* curl_handler_;
    char m_curlmsg[CURL_ERROR_SIZE];
    std::string m_host;
    bool m_inited;
    int m_cache_value;
    std::tr1::unordered_map<std::string, int> m_cache_items;
    int64_t m_last_write;
};

}
}/* namespace ms */

#endif  // BRAND_COUNTER_H_


