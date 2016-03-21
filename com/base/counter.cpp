#include <stdlib.h>
#include <sstream>
#include <string>
#include <sharelib/util/string_util.h>
#include <sharelib/util/time_utility.h>
#include <matchserver/mscom/base/counter.h>
#include <matchserver/mscom/base/log.h>

using namespace std;
using namespace sharelib;
namespace ms {
namespace common{

Counter::Counter(const string &host, int cache_value ) {
    Init( host, cache_value );
}

Counter::Counter() {
    curl_handler_ = NULL;
    m_inited = false;
}

Counter::~Counter() {
    Clean();
}

void Counter::Clean() {
    if ( curl_handler_ ) {
        curl_easy_cleanup( curl_handler_ );
        curl_handler_ = NULL;
    }
    m_inited = false;
}

int Counter::Init(const string &host, int cache_value ) {
    if ( host.empty() ) {
        return -1;
    }

    Clean();
    m_host = host;
    curl_handler_ = curl_easy_init();
    if ( curl_handler_ == NULL ) {
        return -1;
    }
    curl_easy_setopt( curl_handler_, CURLOPT_ERRORBUFFER, m_curlmsg );
    m_inited = true;
    m_last_write = TimeUtility::CurrentTimeInMs();
    m_cache_value = cache_value;
    if ( m_cache_value <= 0 ) {
        m_cache_value = 1000;
    }
    std::cerr << "Counter cache value:" << m_cache_value << std::endl;
    return 0;
}

string Counter::GenerateKey(const string &adid, const string &pdps) {
    if ( adid.empty() || pdps.empty() ) {
        return "";
    }
    return pdps + "-" + adid;
}

bool Counter::ParseKey(const string &key, string& adid, string& pdps ) {
    size_t index = key.find('-');
    if ( index <= 0 || index >= key.length()-1 ) {
        return false;
    }
    pdps = string(key, 0, index);
    adid = string(key, index+1);
    return true;
}

int Counter::Insert2Cache(const std::string &adid, const std::string &value, const std::string &pdps) {
    string key = GenerateKey(adid, pdps);
    if ( key.empty() || value.empty() ) {
        return -1;
    }

    int num = 0;
    if( sscanf( value.c_str(), "%d", &num ) == EOF ) {
        return -1;
    }
    std::tr1::unordered_map<string, int>::iterator iter = m_cache_items.find(key);
    if ( iter != m_cache_items.end() ) {
	    NOTICE(LOGROOT,"counter cache found,increaes from %d to %d", iter->second , iter->second+num);
        iter->second += num;
    } else {
        m_cache_items[key] = num;
        iter = m_cache_items.find(key);
    }

    NOTICE(LOGROOT,"counter cache,key:%s, num:%d,value(%d,%d)", key.c_str(),num,iter->second , m_cache_value);
    if ( iter->second >= m_cache_value ) {
        if ( SendDataToRemote(key, iter->second) == 0 ) {
            m_cache_items.erase(key);
            return 0;
        }
        m_cache_items.erase(key);
        return -1;
    }
    return 0;
}

void Counter::FlushAll() {
	DEBUG(LOGROOT,"counter begin to flush all data");
    std::tr1::unordered_map<string, int>::iterator iter = m_cache_items.begin();
    char buf[32];
    sprintf(buf, "%d", m_cache_items.size() );
    for ( ; iter != m_cache_items.end(); ++iter ) {
        sprintf( buf, "%d", iter->second );
        if ( SendDataToRemote( iter->first, iter->second ) != 0 ) {
	WARN(LOGROOT,"counter flush all data failed");
	}
    }
    m_cache_items.clear();
    m_last_write = TimeUtility::CurrentTimeInMs();
	NOTICE(LOGROOT,"counter flush all data finished");
    return;
}

int Counter::SendDataToRemote(const string &key, int val ) {
    NOTICE(LOGROOT, "counter--->begin");
    if ( m_inited == false ) {
        return -1;
    }

    ostringstream s;
    s << val;
    string value = s.str();
    std::tr1::unordered_map<string, int>::iterator itr = m_cache_items.find(key);
    if ( itr != m_cache_items.end() ) {
        string adid, pdps;
        if ( ParseKey(key, adid, pdps) == true ) {
            char url[1024] = {'\0'};
            snprintf( url, 1024, "%s/counter?key=%s&value=%s&pdps=%s", 
			    m_host.c_str(), adid.c_str(), value.c_str() ,pdps.c_str());
            CURLcode res = curl_easy_setopt( curl_handler_, CURLOPT_URL, url );
            if ( res != CURLE_OK ) {
		   DEBUG(LOGROOT,"counter set curl opt failed,ret code:%d",res);
                return -1;
            }

            res = curl_easy_perform( curl_handler_ );
            if ( res != CURLE_OK ) {
		   WARN(LOGROOT,"counter curl failed,ret code:%d,url:%s",res,url);
                return -1;
	    }
	    DEBUG(LOGROOT,"counter curl ok,ret code:%d,url:%s",res,url);
            return 0;
        }
    }

    return -1;
}

int Counter::Send( const string& key, const string& value, const string& pdps ) {
    int ret = Insert2Cache(key, value, pdps);

    int64_t now = TimeUtility::CurrentTimeInMs();
    if ( (now-m_last_write) >= (1000*60*60) ) { // 1 hour
        FlushAll();
    }

    return ret;
}

}
}/* namespace ms */

