// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/rbtree_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include "os/memalloc.h"
#include "base/skrb.h"
#include "base/ngx_rb.h"


using namespace std;
using namespace nspio;

/* ------- has bug. deprecated rbtree. see krbtree and skrb instead ------- 

static int rbtree_test_single(int argc, char **argv) {
    int i, cnt = 10000;
    std::vector<int64_t> allval;
    int64_t min_val = 0;
    rbtree_t tree;
    rbtree_node_t sentinel, *node, *min_node;

    rbtree_init(&tree, &sentinel, rbtree_insert_value);
    for (i = 0; i < cnt; i++) {
	EXPECT_TRUE((node = (rbtree_node_t *)mem_zalloc(sizeof(rbtree_node_t))) != NULL);
	node->key = rand() + 1;
	if (min_val == 0 || node->key < min_val)
	    min_val = node->key;
	allval.push_back(node->key);
	rbtree_insert(&tree, node);
	min_node = rbtree_min(tree.root, &sentinel);
	EXPECT_TRUE(min_node->key == min_val);
    }

    std::sort(allval.begin(), allval.end());
    for (std::vector<int64_t >::iterator it = allval.begin(); it != allval.end(); ++it) {
	min_node = rbtree_min(tree.root, &sentinel);
	EXPECT_TRUE(min_node->key == *it);
	rbtree_delete(&tree, min_node);
	free(min_node);
    }
    return 0;
}


static int rbtree_test_time_value(int argc, char **argv) {
    rbtree_t tree;
    rbtree_node_t sentinel;

    rbtree_init(&tree, &sentinel, rbtree_insert_timer_value);

    return 0;
}


TEST(rbtree, single) {
    EXPECT_EQ(0, rbtree_test_single(1, NULL));
}


TEST(rbtree, time_value) {
    EXPECT_EQ(0, rbtree_test_time_value(1, NULL));
}

 ------- has bug. deprecated rbtree. see krbtree and skrb instead ------- */

static int skrb_test_single() {
    int i, cnt = 10000;
    std::vector<int64_t> allval;
    int64_t min_val = 0;
    skrb_t tree;
    skrb_node_t *node = NULL, *min_node = NULL;

    skrb_init(&tree);

    for (i = 0; i < cnt; i++) {
	EXPECT_TRUE((node = (skrb_node_t *)mem_zalloc(sizeof(skrb_node_t))) != NULL);
	node->key = rand() + 1;
	if (min_val == 0 || node->key < min_val)
	    min_val = node->key;
	allval.push_back(node->key);
	skrb_insert(&tree, node);
	min_node = skrb_min(&tree);
	EXPECT_TRUE(min_node->key == min_val);
    }

    std::sort(allval.begin(), allval.end());
    for (std::vector<int64_t >::iterator it = allval.begin(); it != allval.end(); ++it) {
	min_node = skrb_min(&tree);
	EXPECT_TRUE(min_node->key == *it);
	skrb_delete(&tree, min_node);
	free(min_node);
    }
    return 0;
}

TEST(skrb, single) {
    EXPECT_EQ(0, skrb_test_single());
}
