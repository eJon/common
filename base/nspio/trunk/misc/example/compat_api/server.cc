#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nspio/compat_api.h>


using namespace nspio;


static string appname = "testapp";
static string apphost = "127.0.0.1:1520";


static int handler_siginit() {
    // Ignore SIGPIPE
    if (SIG_ERR == signal(SIGPIPE, SIG_IGN))
        return -1;
    return 0;
}


int main(int argc, char **argv) {
    CSpioApi server;
    int ret = 0;

    handler_siginit();
    server.init(appname);
    server.join_server(apphost);

    while (1) {
	sleep(1);
	// Read request msg
	string msg;
	if ((ret = server.recv(msg, 5000)) == 0) {
	    server.send(msg, 5000);
	    fprintf(stdout, "server recv: %s\n", msg.c_str());
	} else {
	    fprintf(stderr, "server recv with errno %d\n", errno);
	}
    }
    return 0;
}
