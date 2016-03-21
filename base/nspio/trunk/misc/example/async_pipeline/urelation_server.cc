#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nspio/async_api.h>

using namespace nspio;


static string urelationname = "urelation";
static string urelationhost = "127.0.0.1:11510";


class UserRelation_RequestHandler : public RequestHandler {
public:
    int HandleRequest(const char *data, uint32_t len, ResponseWriter &rw);
    string queryDB(string &usr);

private:
};

string UserRelation_RequestHandler::queryDB(string &usr) {
    return " mock_relationship:nothing ";
}

int UserRelation_RequestHandler::HandleRequest(const char *data, uint32_t len, ResponseWriter &rw) {
    string msg(data, len);
    string response;

    response = queryDB(msg);
    rw.Send(response.data(), response.size());
    cout << "urelation query req {" << msg << "}" << endl;
    return 0;
}


int main(int argc, char **argv) {
    UserRelation_RequestHandler urr;
    async_conf conf;
    AsyncComsumer *asc = NewAsyncComsumer();

    conf.appname = urelationname;
    conf.apphost = urelationhost;
    asc->Setup(conf, &urr);
    asc->StartServe(); 

    while (1)
	sleep(1);

    asc->Stop();
    delete asc;
    return 0;
}
