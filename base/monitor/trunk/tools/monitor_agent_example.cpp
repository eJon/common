#include <monitor/client/client_agent.h>
#include <sharelib/util/log.h>
#include <string>
#include <iostream>
using namespace monitor;
using namespace std;
//use sharelib(log4cpp)
int main(int argc, char**argv ){
    
    if(argc != 3){
        cout << "uasge:./client_agent_example client_conf log4cpp.conf" << endl;
        return -1;
    }
    LOG_CONFIG(argv[2]);
    string conf = string(argv[1]);
    cout << "conf is " << conf <<endl;
    ClientAgent* monitorClient = new ClientAgent();
    if(0 !=  monitorClient->Init(conf)){
        cout << "client init error" << endl;
        return -1;
    }
    
    
    for(uint32_t i = 0; i < 10000000;i++){
        stringstream ss;
        ss<< "snapshot_info_"<< i;
        string str = ss.str();
        stringstream ssd;
        ssd << "dataid" << (i%100);
        string dataid = ssd.str();
        monitorClient->SendSnapshot(str, dataid);
        stringstream ssk;
        ssk << "key" << (i %100);
        string key = ssk.str();
        monitorClient->Add(key, 1);
        if(i % 1000 == 0)sleep(1);
    }
    sleep(10);
    delete monitorClient;
    LOG_SHUTDOWN();
    return 0;
}
