#ifndef MONITOR_CLIENT_MONITOR_H
#define MONITOR_CLIENT_MONITOR_H

#include <sharelib/task/concurrent_queue.h>
#include <monitor/common.h>
#include <monitor/common/monitor_info.h>
MONITOR_BS;

typedef std::tr1::shared_ptr<CurrentQueue<MonitorInfoPtr> > InfoCurrentQueuePtr;

enum CumulationType{
    add =0,
    deduce=1
};

class KeyValue{
public:
    KeyValue(std::string& keyIn, int64_t valueIn,CumulationType typeIn):key(keyIn), value(valueIn), type(typeIn){}
    std::string ToString(){
        std::stringstream ss;
        ss << key << ":" << value;
        return ss.str();
    }
public:
    std::string key;
    int64_t value;
    CumulationType type;
};
typedef std::tr1::shared_ptr<KeyValue> KeyValuePtr;

class KeyValueManager{
public:
    typedef std::map<std::string, int64_t>::iterator MapIt;
public:
    void Reset(){
        kvs.clear();
    }
    std::string ToTextString(){
        if(kvs.size() == 0) return "";
        std::stringstream ss;
        ss << "|" ;
        MapIt it;
        for(it = kvs.begin();it != kvs.end(); it++){
            ss << it->first << ":" << it->second;
            ss << "|";
        }
        return ss.str();
    }
    
    void Caculate(KeyValuePtr kv){
        MapIt it = kvs.find(kv->key);
        if(it == kvs.end()){
            kvs[kv->key] = kv->value;
        }else{
            switch(kv->type)
            {
            case add:
            {
                it->second = it->second + kv->value;
                break;
            }
            case deduce:
            {
                it->second = it->second - kv->value;
                break;
            }
            }    
        }
        
    }
public:
    std::map<std::string, int64_t> kvs;
};

typedef std::tr1::shared_ptr<KeyValueManager> KeyValueManagerPtr;

typedef std::tr1::shared_ptr<CurrentQueue<KeyValuePtr> > KvCurrentQueuePtr;

typedef std::tr1::shared_ptr<CurrentQueue<MonitorInfoPtr> > InfoCurrentQueuePtr;



MONITOR_ES;
#endif //MONITOR_MONITOR_H
