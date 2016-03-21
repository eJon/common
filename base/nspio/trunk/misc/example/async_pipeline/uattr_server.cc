#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nspio/async_api.h>

using namespace nspio;


static string uattrname = "uattr";
static string uattrhost = "127.0.0.1:11510";


class UserAttr_RequestHandler : public RequestHandler {
public:
    int HandleRequest(const char *data, uint32_t len, ResponseWriter &rw);
    string queryDB(string &usr);

private:
};

string UserAttr_RequestHandler::queryDB(string &usr) {
    return " mock_age:18 mock_sex:female ";
}


int UserAttr_RequestHandler::HandleRequest(const char *data, uint32_t len, ResponseWriter &rw) {
    string msg(data, len);
    string response;

    response = queryDB(msg);
    rw.Send(response.data(), response.size());
    cout << "uattr query req {" << msg << "}" << endl;
    return 0;
}


int main(int argc, char **argv) {
    UserAttr_RequestHandler uar;
    async_conf conf;
    AsyncComsumer *asc = NewAsyncComsumer();

    conf.appname = uattrname;
    conf.apphost = uattrhost;
    asc->Setup(conf, &uar);
    asc->StartServe(); 

    while (1)
	sleep(1);

    asc->Stop();
    delete asc;
    return 0;
}
