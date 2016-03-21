// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/base/skrb.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _H_SKRB_
#define _H_SKRB_

#include <stdint.h>
#include <inttypes.h>
#include "base/krb.h"

using namespace nspio;

NSPIO_DECLARATION_START

struct skrb_node_t {
    struct rb_node  rb;
    int64_t         key;
    void            *data;
};

struct skrb_t {
    int64_t elem_size;
    struct rb_root root;
};

#define skrb_init(tree)	do {			\
	INIT_RB_ROOT(&(tree)->root);		\
	(tree)->elem_size = 0;			\
    } while (0)

#define skrb_empty(tree) RB_EMPTY_ROOT(&(tree)->root)


static inline skrb_node_t *skrb_min(skrb_t *tree) {
    struct rb_root *root = &tree->root;
    struct rb_node *cur = root->rb_node, *parent = NULL;
    struct skrb_node_t *_min = NULL;
    
    while (cur) {
	parent = cur;
	cur = parent->rb_left;
	_min = rb_entry(parent, struct skrb_node_t, rb);
    }
    return _min;
}


static inline skrb_node_t *skrb_max(skrb_t *tree) {
    struct rb_root *root = &tree->root;
    struct rb_node *cur = root->rb_node, *parent = NULL;
    struct skrb_node_t *_max = NULL;
    
    while (cur) {
	parent = cur;
	cur = parent->rb_right;
	_max = rb_entry(parent, struct skrb_node_t, rb);
    }
    return _max;
}

static inline void skrb_insert(skrb_t *tree, skrb_node_t *node) {
    struct rb_root *root = &tree->root;
    struct rb_node **np = &root->rb_node, *parent = NULL;
    int64_t key = node->key;

    tree->elem_size++;
    while (*np) {
	parent = *np;
	if (key < rb_entry(parent, struct skrb_node_t, rb)->key)
	    np = &parent->rb_left;
	else
	    np = &parent->rb_right;
    }

    rb_link_node(&node->rb, parent, np);
    rb_insert_color(&node->rb, root);
}


static inline void skrb_delete(skrb_t *tree, skrb_node_t *node) {
    tree->elem_size--;
    rb_erase(&node->rb, &tree->root);
}

}
#endif /* _H_SKRB_ */
