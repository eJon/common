// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/path_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <gtest/gtest.h>
#include "path/filepath.h"

using namespace nspio;

static int entries_cnt = 0;

static void my_walkfn(const string &path, void *data) {
    entries_cnt++;
}


static int path_func_test() {
    EXPECT_TRUE(Dir("") == "");
    EXPECT_TRUE(Base("") == "");
    EXPECT_TRUE(Dir("gtest.h") == "");
    EXPECT_TRUE(Base("gtest.h") == "gtest.h");
    EXPECT_TRUE(Dir("./gtest.h") == "./");
    EXPECT_TRUE(Base("./gtest.h") == "gtest.h");
    EXPECT_TRUE(Dir("/tmp/gtest/gtest.h") == "/tmp/gtest/");
    EXPECT_TRUE(Base("/tmp/gtest/gtest.h") == "gtest.h");
    EXPECT_TRUE(Abs("/tmp/gtest/gtest.h") == "/tmp/gtest/gtest.h");
    EXPECT_TRUE(HasPrefix("/tmp/gtest", "/tmp"));
    EXPECT_TRUE(HasPrefix("/tmp/gtest", ""));
    EXPECT_TRUE(HasPrefix("/tmp/gtest", "/tmp/gtest"));
    EXPECT_TRUE(!HasPrefix("/tmp/gtest", "/tmp/gtest/"));
    EXPECT_TRUE(HasSuffix("/tmp/gtest", "gtest"));
    EXPECT_TRUE(HasSuffix("/tmp/gtest", ""));
    EXPECT_TRUE(HasSuffix("/tmp/gtest", "/tmp/gtest"));
    EXPECT_TRUE(!HasSuffix("/tmp/gtest", "/tmp/gtest/"));
    return 0;
}

static int path_walk_test() {
    FilePath fp;
    fp.Setup(".");
    fp.WalkDir(my_walkfn);
    fp.DeepWalkDir(my_walkfn, -1);
    EXPECT_TRUE(entries_cnt != 0);
    return 0;
}


TEST(path, walk) {
    path_walk_test();
    path_func_test();
}
