// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: wangqiushi@baidu.com
// Date: 2012-07-09 00:00

#include <gtest/gtest.h>
#include "cow_table.hpp"
#include "unique_index.hpp"
#include "unique_cluster_index.hpp"

using namespace st;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class test_basic_suite : public ::testing::Test{
protected:
    test_basic_suite(){};
    virtual ~test_basic_suite(){};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};


struct sign_t {
    sign_t() {}
    sign_t(int s1, int s2) : sign1(s1), sign2(s2) {}
    int sign1;
    int sign2;
};


DEFINE_ATTRIBUTE(A, int);
DEFINE_ATTRIBUTE(B, unsigned int);
DEFINE_ATTRIBUTE(C, double);
DEFINE_ATTRIBUTE(D, sign_t);

typedef ST_TABLE(A, B, C, D, ST_UNIQUE_KEY(A), ST_UNIQUE_KEY(B, ST_CLUSTER_KEY(A))) T;

TEST_F(test_basic_suite, test_dump_load)
{
    system("rm -f test_dump_load.tmp");
    
    T t;    
    ASSERT_EQ(0, t.init());

    // Insert a few pairs, number of parameters should match definition of `T'
    t.insert(11, 12, 1.1, sign_t(1, 1));
    t.insert(21, 22, 2.2, sign_t(2, 2));
    t.insert(31, 32, 2.3, sign_t(3, 3));
    t.insert(41, 42, 2.3, sign_t(3, 3));
    t.insert(51, 52, 2.3, sign_t(3, 3));

    const char* dir = "./";
    const char* filename = "test_dump_load.tmp";
    const char* table_name = "test_dump_load_table";

    // dump
    st::MetaData m_data;
    m_data.version = 1;
    m_data.set_name(table_name);
    m_data.partition = -1;
    m_data.item_size = 24;
    m_data.item_count = 3;
    
    ASSERT_EQ(0, t.dump(dir, filename, m_data));
    
    for (int i = 1; i < 100; i++) {
        T loaded_table;
        ASSERT_EQ(0, loaded_table.init());
        ASSERT_EQ(0, loaded_table.load(dir, filename, m_data, i));

        ASSERT_TRUE((loaded_table.seek<A>(11)));
        ASSERT_TRUE((loaded_table.seek<A>(21)));
        ASSERT_TRUE((loaded_table.seek<A>(31)));
        ASSERT_TRUE((loaded_table.seek<A>(41)));
        ASSERT_TRUE((loaded_table.seek<A>(51)));
    }
    
    system("rm -f test_dump_load.tmp");
}

TEST_F(test_basic_suite, test_binary_file_reader)
{
    system("rm -f test_binary_file_reader.tmp");

    T t;
    ASSERT_EQ(0, t.init());

    const char* dir = "./";
    const char* filename = "test_binary_file_reader.tmp";
    const char* table_name = "test_binary_file_reader";

    // dump
    st::MetaData m_data;
    m_data.version = 1;
    m_data.set_name(table_name);
    m_data.partition = -1;
    m_data.item_size = 24;
    m_data.item_count = 3;
    
    ASSERT_EQ(0, t.dump(dir, filename, m_data));

    // read an binary file with only meta data
    BinaryFileReader reader(1);
    ASSERT_EQ(0, reader.init());

    CommonMetaReader<MetaData> meta_reader;
    ASSERT_EQ(0, reader.open(dir, filename, &meta_reader));
    ASSERT_TRUE(NULL == reader.get_value());

    system("rm -f test_binary_file_reader.tmp");
}
