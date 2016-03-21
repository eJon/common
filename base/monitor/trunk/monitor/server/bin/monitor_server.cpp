#include <monitor/server/monitor_epoll_server.h>
#include <monitor/common/log.h>
#include <monitor/server/bin/monitor_server_conf.h>
using namespace std;
using namespace monitor;
using namespace sharelib;

int main(int argc, char** argv)
{
    if(argc != 2){
        cout << "Usage: ./monitorserver confpath"<< endl;
        return -1;
    }
    string confpath = string(argv[1]);
    string log4cpp  = confpath + "/log4cpp.conf";
    
    
    MonitorServerConfPtr serverConf = MonitorServerConf::ReadFromFile(confpath + "/monitor_server.conf");
    if(serverConf == NULL){
        cout << "monitor server init error" << endl;
        return -1;
    }

    LOG_CONFIG(log4cpp);
    AppConfManagerPtr appConfManager(new AppConfManager());
    if (0 != appConfManager->Init(serverConf->appConfDir)){
        cout <<"app conf manager init error" << endl;
        return -1;
    }

    MonitorTaskHandlerPtr taskHandler(new MonitorTaskHandler);
    taskHandler->SetAppConfManager(appConfManager);
    if(0 != taskHandler->Init(serverConf->queueSize, serverConf->threadNum)|| 0 != taskHandler->Start())
    {
        cout << "task handler init error" << endl;
        return -1;        
    }
    
    
    MonitorEpollServer* server = new MonitorEpollServer();
    server->SetTaskHandler(taskHandler);
    if(0 != server->Init(serverConf->port, "")){
        cout << "epoll server init error" << endl;
        delete server;
        return -1;
    }
    server->Run();

    while(true){
        MONITOR_LOG_INFO("monitor server threading running");
        sleep(3);
    }
    
    
    server->Shutdown();
    server->Stop();
    delete server;
    
    LOG_SHUTDOWN();
    return 0;
}



