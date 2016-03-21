#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nspio/async_api.h>

using namespace nspio;

static string frontname = "front";
static string fronthost = "127.0.0.1:11510";

class UserInfo_ResponseHandler : public ResponseHandler {
public:
    int HandleError(Error &ed, void *cb);
    int HandleResponse(const char *data, uint32_t len, void *cb);

private:
};

int UserInfo_ResponseHandler::HandleError(Error &ed, void *cb) {
    char *req = (char *)cb;
    cout << "query with error " << ed.Str() << endl;
    free(req);
    return 0;
}

int UserInfo_ResponseHandler::HandleResponse(const char *data, uint32_t len, void *cb) {
    char *req = (char *)cb;
    string response(data, len);

    cout << "query result {" << response << "}" << endl;
    free(req);
    return 0;
}

int main(int argc, char **argv) {
    char *cb = NULL;
    string msg("username:sina");
    UserInfo_ResponseHandler h;
    AsyncProducer *asp = NewAsyncProducer();
    async_conf conf;

    conf.set(frontname, fronthost, 0, 0, 0);
    asp->Setup(conf, &h);
    asp->StartServe();

    while (1) {
	cb = strndup(msg.data(), msg.size());
	if (asp->SendRequest(msg.data(), msg.size(), cb, 2 /* 2ms timeout */) < 0)
	    cout << "query with errno " << errno << endl;
	else
	    cout << "query {" << msg << "}" << endl;
	sleep(1);
    }

    asp->Stop();
    delete asp;
    return 0;
}




