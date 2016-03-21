// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Test hd_map.h
// Author: gejun@baidu.com

#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <ext/hash_map>
#include "hd_map.hpp"
#include "st_timer.h"

using namespace __gnu_cxx;
using namespace st;
	
int main(int argc, char **argv)
{
    srand (time(0));
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class test_usage_suite : public ::testing::Test{
    protected:
        test_usage_suite(){};
        virtual ~test_usage_suite(){};
        virtual void SetUp() {
        };
        virtual void TearDown() {
        };
};

TEST_F(test_usage_suite, sanity)
{
    typedef HDMap<int> Map;
    typedef int Value;
    Map m;
    Value input = 10;
    Value* output = NULL;
    size_t output_length = sizeof(Value);
    
    ASSERT_TRUE(m.not_init());
    m.init("value_file", 1000, 70);
    ASSERT_FALSE(m.not_init());
    ASSERT_EQ(m.size(), 0ul);
    ASSERT_TRUE(m.empty());

    // Initial insertion
    m.insert(1, &input, sizeof(Value));
    ASSERT_EQ(m.size(), 1ul);
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(m.seek(1, (void**)&output, &output_length), sizeof(Value));
    ASSERT_EQ(*output, input);

    // Override
    input = 100;
    m.insert(1, &input, sizeof(Value));
    ASSERT_EQ(m.size(), 1ul);
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(m.seek(1, (void**)&output, &output_length), sizeof(Value));
    ASSERT_EQ(*output, input);
    
    // Insert anthter
    input = 20;
    m.insert(2, &input, sizeof(Value));
    ASSERT_EQ(m.size(), 2ul);
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(m.seek(2, (void**)&output, &output_length), sizeof(Value));
    ASSERT_EQ(*output, input);

    // Erase exist
    m.erase(1);
    ASSERT_EQ(m.size(), 1ul);
    ASSERT_FALSE(m.empty());
    ASSERT_FALSE(m.exist(1));

    // Clear
    m.clear();
    ASSERT_EQ(m.size(), 0ul);
    ASSERT_TRUE(m.empty());
    ASSERT_FALSE(m.exist(1));
    ASSERT_FALSE(m.exist(2));
    
    system("rm -f value_file");
}


// // Randomly insert/erase/clear a HDMap while making snapshots
// TEST_F(test_usage_suite, random_insert_erase)
// {
//     srand (time(0));

//     hash_map<int, Value> ref[2];
//     typedef HDMap<int, Value> Map;
//     Map ht[2];
//     ht[0].init (40, 80);
//     cout << show(ht[0]) << endl;
        
//     ht[1] = ht[0];
//     cout << show(ht[1]) << endl;

//     for (int j = 0; j < 30; ++j) {
//         // Make snapshot
//         ht[1] = ht[0];
//         ref[1] = ref[0];

//         for (int i = 0; i < 100000; ++i) {
//             int k = rand() % 0xFFFF;
//             int p = rand() % 1000;
//             if (p < 600) {
//                 ht[0].insert(k, i);
//                 ref[0][k] = i;
//             } else if(p < 999) {
//                 ht[0].erase (k);
//                 ref[0].erase (k);
//             } else {
//                 ht[0].clear();
//                 ref[0].clear();
//             }
//         }
        
//         // bi-check
//         for (int i=0; i<2; ++i) {
//             for (HDMap<int, Value>::Iterator it = ht[i].begin();
//                  it != ht[i].end(); ++it)
//             {
//                 hash_map<int, Value>::iterator it2 = ref[i].find(it->first);
//                 ASSERT_TRUE (it2 != ref[i].end());
//                 ASSERT_EQ (it2->second, it->second);
//             }
        
//             for (hash_map<int, Value>::iterator it = ref[i].begin();
//                  it != ref[i].end(); ++it)
//             {
//                 Value* p_value = ht[i].seek(it->first);
//                 ASSERT_TRUE (p_value != NULL);
//                 ASSERT_EQ (it->second, p_value->x_);
//             }
        
//             ASSERT_EQ (ht[i].size(), ref[i].size());
//         }
//     }

//     cout << "ht[0] = " << show(ht[0]) << endl
//          << "ht[1] = " << show(ht[1]) << endl;

//     ht[0].clear ();
//     ht[1].clear ();
//     ref[0].clear ();
//     ref[1].clear ();

//     ASSERT_EQ (ht[0].alloc()->alloc_num(), 0ul);
//     ASSERT_EQ (n_con + n_cp_con, n_des);

//     cout << "n_con:" << n_con << endl
//          << "n_cp_con:" << n_cp_con << endl
//          << "n_con+n_cp_con:" <<  n_con+n_cp_con <<  endl
//          << "n_des:" << n_des << endl
//          << "n_cp:" << n_cp << endl
//         ;    
// }


