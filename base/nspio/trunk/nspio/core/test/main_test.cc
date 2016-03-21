// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/test/main_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include <sys/signal.h>
#include <nspio/test/test.h>
#include "ut.h"
#include "os/os.h"
#include "log.h"
#include "ctx_global.h"

using namespace nspio;

int gargc = 0;
char **gargv = NULL;

int main(int argc, char **argv) {

    int ret;
    
    gargc = argc;
    gargv = argv;


    // Ignore SIGPIPE
    if (SIG_ERR == signal(SIGPIPE, SIG_IGN)) {
        fprintf(stderr, "signal SIG_IGN");
        return -1;
    }
    testing::InitGoogleTest(&gargc, gargv);

    NSPIOLOG_CONFIG(DOTEST_LOGGER_CONF);

    nspio_os_init();
    set_ctx_global_stat_recorder(NULL);
    ret = RUN_ALL_TESTS();

    NSPIOLOG_SHUTDOWN();
    return ret;
}
