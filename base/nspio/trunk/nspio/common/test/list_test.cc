// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/test/list_test.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "base/list.h"


using namespace std;
using namespace nspio;


struct list_node {
    int data;
    struct list_head link;
};

static int nodecnt;
static int hnodecnt;

struct list_node *list_node_new() {
    struct list_node *node;
    if ((node = (struct list_node *)malloc(sizeof(*node)))) {
	nodecnt++;
	memset(node, 0, sizeof(*node));
    }
    return node;
}

void list_node_free(struct list_node *node) {
    nodecnt--;
    free(node);
} 

static int test_list() {
	int i, n = 10000;
	int sum = 0;
	struct list_head head, newhead;
	struct list_node *node, *tmpnode;


	INIT_LIST_HEAD(&head);
	INIT_LIST_HEAD(&newhead);
	
	for (i = 0; i < n; i++) {
		node = list_node_new();
		node->data = i;
		if (!node) {
			break;
		}
		list_add(&node->link, &head);
	}

	list_for_each_entry(node, &head, struct list_node, link) {
		sum += node->data;
	}
	EXPECT_TRUE(sum == ((0 + n - 1) * n)/2);

	list_for_each_entry_safe(node, tmpnode, &head, struct list_node, link) {
		if (list_empty(&newhead) || rand() % 2 == 0)
			list_move(&node->link, &newhead);
	}

	list_for_each_entry_safe(node, tmpnode, &head, struct list_node, link) {
		list_del(&node->link);
		list_node_free(node);
	}
	EXPECT_TRUE(list_empty(&head));

	list_for_each_entry_safe(node, tmpnode, &newhead, struct list_node, link) {
		list_move_tail(&node->link, &head);
	}

	list_for_each_entry_safe(node, tmpnode, &head, struct list_node, link) {
		if (list_empty(&newhead) || rand() % 2 == 0)
			list_move(&node->link, &newhead);
	}

	EXPECT_TRUE(!list_empty(&newhead));
	EXPECT_TRUE(!list_empty(&head));
	list_splice(&head, &newhead);
	EXPECT_TRUE(list_empty(&head));

	list_for_each_entry_safe(node, tmpnode, &newhead, struct list_node, link) {
		list_del(&node->link);
		list_node_free(node);
	}
	EXPECT_EQ(0, nodecnt);
	EXPECT_TRUE(list_empty(&head));
	EXPECT_TRUE(list_empty(&head));

	return 0;
}


static int list_benchmark() {
    int i = 0, cnt = 100000;
    struct list_head head, newhead;
    struct list_node *node, *tmpnode;
    
    INIT_LIST_HEAD(&head);
    INIT_LIST_HEAD(&newhead);
    for (i = 0; i < cnt; i++) {
	node = list_node_new();
	node->data = i;
	list_add(&node->link, &head);
    }
    list_for_each_entry_safe(node, tmpnode, &head, struct list_node, link) {
	list_move(&node->link, &newhead);
    }
    list_for_each_entry_safe(node, tmpnode, &newhead, struct list_node, link) {
	list_del(&node->link);
	list_node_free(node);
    }
    return 0;
}


static int stllist_benchmark() {
    int i = 0, cnt = 100000;
    struct list_node *node = NULL;
    vector<struct list_node *> head, newhead;
    vector<struct list_node *>::iterator it;
    
    for (i = 0; i < cnt; i++) {
	node = list_node_new();
	node->data = i;
	head.push_back(node);
    }
    for (it = head.begin(); it != head.end(); ++it) {
	node = *it;
	newhead.push_back(node);
    }
    for (it = newhead.begin(); it != newhead.end(); ++it) {
	node = *it;
	list_node_free(node);
    }
    return 0;
}



TEST(list, list) {
    test_list();
}

TEST(list, bc) {
    list_benchmark();
}

TEST(list, stlbc) {
    stllist_benchmark();
}






struct mynode {
    int data;
    struct hlist_node link;
};

struct mynode *mynode_new() {
    struct mynode *node;
    if ((node = (struct mynode *)malloc(sizeof(*node)))) {
	hnodecnt++;
	memset(node, 0, sizeof(*node));
    }
    return node;
}

void mynode_free(struct mynode *node) {
    hnodecnt--;
    free(node);
} 



int test_hlist() {
    int i, n = 10000;
    int sum = 0, __cnt;
    // the list header
    struct hlist_head head, newhead;
    struct mynode *node, *tmpnode;


    INIT_HLIST_HEAD(&head);
    INIT_HLIST_HEAD(&newhead);
	
    for (i = 0; i < n; i++) {
	node = mynode_new();
	node->data = i;
	if (!node) {
	    break;
	}
	hlist_add_head(&node->link, &head);
    }

    hlist_for_each_entry(node, &head, struct mynode, link) {
	sum += node->data;
    }

    EXPECT_TRUE((sum == ((0 + n - 1) * n)/2));
    hlist_for_each_entry_safe(node, tmpnode, &head, struct mynode, link) {
	if (hlist_empty(&newhead) || rand() % 2 == 0) {
	    hlist_del(&node->link);
	    hlist_add_head(&node->link, &newhead);
	}
    }

    __cnt = 0;
    hlist_for_each_entry_safe(node, tmpnode, &head, struct mynode, link) {
	__cnt += node->data;
    }
	
    hlist_for_each_entry_safe(node, tmpnode, &head, struct mynode, link) {
	hlist_del(&node->link);
	mynode_free(node);
    }
    __cnt = 0;
    hlist_for_each_entry_safe(node, tmpnode, &head, struct mynode, link) {
	__cnt += node->data;
    }
    EXPECT_TRUE(hlist_empty(&head));

    hlist_for_each_entry_safe(node, tmpnode, &newhead, struct mynode, link) {
	hlist_del(&node->link);
	if (hlist_empty(&head))
	    hlist_add_head(&node->link, &head);
	else
	    hlist_add_after(&node->link, head.first);
    }

    hlist_for_each_entry_safe(node, tmpnode, &head, struct mynode, link) {
	if (hlist_empty(&newhead) || rand() % 2 == 0) {
	    hlist_del(&node->link);
	    if (hlist_empty(&newhead))
		hlist_add_head(&node->link, &newhead);
	    else
		hlist_add_before(&node->link, newhead.first);
	}
    }
    EXPECT_TRUE(!hlist_empty(&head));
    EXPECT_TRUE(!hlist_empty(&newhead));

    hlist_for_each_entry_safe(node, tmpnode, &newhead, struct mynode, link) {
	hlist_del(&node->link);
	mynode_free(node);
    }

    hlist_for_each_entry_safe(node, tmpnode, &head, struct mynode, link) {
	hlist_del(&node->link);
	mynode_free(node);
    }

    EXPECT_EQ(0, hnodecnt);
    EXPECT_TRUE(hlist_empty(&head));
    EXPECT_TRUE(hlist_empty(&newhead));
    return 0;
}



TEST(list, hashlist) {
    test_hlist();
}
