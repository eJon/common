// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-12-16 09:13

#include <gtest/gtest.h>
#include <st_timer.h>
#include <version_manager.hpp>
#include <cow_hash_map.hpp>

using namespace st;

class test_usage_suite : public ::testing::Test{
protected:
    test_usage_suite(){};
    virtual ~test_usage_suite(){};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};

TEST_F(test_usage_suite, basic_interfaces)
{
    VersionManager<int> vm;
    ASSERT_EQ (0, vm.init (5, "test_int"));
    ASSERT_EQ (vm.ver_num(), 0u);
    int idx;
    
    cout << show(vm).c_str() << endl;
    idx = vm.create_version();
    ASSERT_TRUE (idx >= 0);
    ASSERT_EQ (vm.ver_num(), 1u);
    cout << show(vm).c_str() << endl;
    ASSERT_TRUE (vm.find_latest_read_only() < 0);
    vm[idx] = 20;
    cout << show(vm).c_str() << endl;

    idx = vm.create_version();
    ASSERT_TRUE (idx >= 0);
    ASSERT_EQ (vm.ver_num(), 2u);
    ASSERT_EQ (vm.find_latest_read_only(), 0);
    cout << show(vm).c_str() << endl;

    idx = vm.create_version();
    ASSERT_TRUE (idx >= 0);
    ASSERT_EQ (vm.ver_num(), 3u);
    vm[idx] = 30;
    ASSERT_EQ (vm.find_latest_read_only(), 1);
    cout << show(vm).c_str() << endl;

    idx = vm.create_version();
    ASSERT_TRUE (idx >= 0);
    ASSERT_EQ (vm.ver_num(), 4u);
    ASSERT_EQ (vm.find_latest_read_only(), 2);
    vm[idx] = 40;
    ASSERT_EQ (0, vm.freeze_version (idx));
    ASSERT_EQ (vm.find_latest_read_only(), 3);
    cout << show(vm).c_str() << endl;

    idx = vm.create_version();
    ASSERT_TRUE (idx >= 0);
    ASSERT_EQ (vm.ver_num(), 5u);
    cout << show(vm).c_str() << endl;

    idx = vm.create_version();
    ASSERT_EQ (vm.ver_num(), 5u);
    ASSERT_TRUE (idx >= 0);
    cout << show(vm).c_str() << endl;
}

typedef CowHashMap<int,int> Map;

TEST_F(test_usage_suite, with_cow_hash_map)
{
    VersionManager<Map> vm;
    ASSERT_EQ (0, vm.init(2, "test_map"));
    int idx;
    
    idx = vm.create_version();
    ASSERT_EQ (idx, 0);

    Map* p_map = &vm[idx];
    p_map->init(1000,80);
    p_map->insert (1,2);
    p_map->insert (2,3);
    cout << show(vm).c_str() << endl;

    idx = vm.create_version();
    ASSERT_EQ (idx, 1);
    p_map = &vm[idx];
    p_map->insert (1,3);
    p_map->insert (3,4);
    cout << show(vm).c_str() << endl;

    idx = vm.create_version();
    ASSERT_EQ (idx, 0);
    p_map = &vm[idx];
    p_map->erase (1);
    p_map->insert (3,5);
    p_map->insert (10,3);
    p_map->insert (30,4);
    cout << show(vm).c_str() << endl;
}

