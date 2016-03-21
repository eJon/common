#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nspio/async_api.h>

using namespace nspio;

static string appname = "testapp";
static string apphost = "127.0.0.1:1510";    

class EchoResponseHandler : public ResponseHandler {
public:
    int HandleError(Error &ed, void *req);
    int HandleResponse(const char *data, uint32_t len, void *req);

private:
};

int EchoResponseHandler::HandleError(Error &ed, void *req) {
    cout << "request deliver with error " << ed.Str() << endl;
    free(req);
    return 0;
}

int EchoResponseHandler::HandleResponse(const char *data, uint32_t len, void *req) {
    if (memcmp(data, (char *)req, len) != 0)
	cout << "recv response with error request != response" << endl;
    free(req);
    return 0;
}

int main(int argc, char **argv) {
    char *req = NULL;
    string msg("i am async client");
    EchoResponseHandler erh;
    async_conf conf;
    AsyncProducer *asp = NewAsyncProducer();

    conf.appname = appname;
    conf.apphost = apphost;
    asp->Setup(conf, &erh);
    asp->StartServe();

    while (1) {
	req = strndup(msg.data(), msg.size());
	if (asp->SendRequest(msg.data(), msg.size(), req, 2 /* 2ms timeout */) < 0)
	    cout << "async client send request with errno " << errno << endl;
	else
	    cout << "async client send request {" << msg << "}" << endl;
	sleep(1);
    }

    asp->Stop();
    delete asp;
    return 0;
}




