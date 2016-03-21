#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nspio/async_api.h>

using namespace nspio;


static string appname = "testapp";
static string apphost = "127.0.0.1:1520";


class EchoRequestHandler : public RequestHandler {
public:
    int HandleRequest(const char *data, uint32_t len, ResponseWriter &rw);

private:
};

int EchoRequestHandler::HandleRequest(const char *data, uint32_t len, ResponseWriter &rw) {
    string msg(data, len);
    rw.Send(data, len);
    cout << "async server recv request {" << msg << "}" << endl;
    return 0;
}


int main(int argc, char **argv) {
    EchoRequestHandler erh;
    async_conf conf;
    AsyncComsumer *asc = NewAsyncComsumer();

    conf.appname = appname;
    conf.apphost = apphost;
    asc->Setup(conf, &erh);
    asc->StartServe(); 

    while (1) {
	sleep(1);
    }

    asc->Stop();
    delete asc;
    return 0;
}
