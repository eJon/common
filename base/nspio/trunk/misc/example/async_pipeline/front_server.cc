#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <nspio/multi_api.h>

using namespace nspio;

enum {
    FRONT_NET = 1,
    UATTR_NET,
    URELATION_NET,
};

static string frontname = "front";
static string fronthost = "127.0.0.1:11510";

static string uattrname = "uattr";
static string uattrhost = "127.0.0.1:11510";

static string urelationname = "urelation";
static string urelationhost = "127.0.0.1:11510";

class mysql_db {
public:
    bool query_exist(const string &user) {
	bool __exist(false);
	lock();
	__exist = true;
	unlock();
	return __exist;
    }
private:
    void lock() {}
    void unlock() {}
};


class query_result : public reqresp_ctx {
public:
    query_result() :
	db(NULL)
    {}
    ~query_result() {}
    void init(mysql_db *db_engine) {
	db = db_engine;
    }
    
    int request_come(const char *data, uint32_t len, MultiSender *s) {
	user.assign(data, len);
	if (db->query_exist(user)) {
	    s->Send(UATTR_NET, data, len, this, 10);
	    s->Send(URELATION_NET, data, len, this, 10);
	}
	return 0;
    }
    int back_response(ResponseWriter &w, bool bad) {
	if (!bad)
	    w.Send(response.data(), response.size());
	return 0;
    }
    int one_response_bad(int who, Error &ed) {
	return 0;
    }
    int one_response_done(int who, const char *data, uint32_t len) {
	response.append(data, len);
	return 0;
    }

private:
    mysql_db *db;
    string user, response;
};

class ms_reqresp_ctxfactor : public reqresp_ctxfactor {
public:
    reqresp_ctx *new_reqresp_ctx() {
	query_result *qr = new (std::nothrow) query_result();
	qr->init(&db_engine);
	return qr;
    }

private:
    mysql_db db_engine;
};

int main(int argc, char **argv) {
    Multiio mio;
    ms_reqresp_ctxfactor f;
    async_conf inapp, outapp;

    inapp.set(frontname, fronthost, 1, 10, 10);
    mio.Init(&f, 2, inapp);
    outapp.set(uattrname, uattrhost, 1, 10, 10);
    mio.AddBackendServer(UATTR_NET, outapp);
    outapp.set(urelationname, urelationhost, 1, 10, 10);
    mio.AddBackendServer(URELATION_NET, outapp);
    mio.Start();
    
    while (1)
	sleep(1);

    mio.Stop();
    return 0;
}
