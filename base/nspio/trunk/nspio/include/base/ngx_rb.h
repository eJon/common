// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/base/ngx_rb.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _RBTREE_H_INCLUDED_
#define _RBTREE_H_INCLUDED_

#include <stdint.h>
#include <inttypes.h>
#include "decr.h"

/* ------- has bug. deprecated rbtree. see krbtree and skrb instead ------- */


NSPIO_DECLARATION_START

typedef unsigned char u_char;

struct rbtree_node_t {
    int64_t       key;
    rbtree_node_t     *left;
    rbtree_node_t     *right;
    rbtree_node_t     *parent;
    u_char            color;
    void              *data;
};

typedef void (*rbtree_insert_pt) (rbtree_node_t *root,
				  rbtree_node_t *node, rbtree_node_t *sentinel);

struct rbtree_t {
    int64_t elem_size;
    rbtree_node_t     *root;
    rbtree_node_t     *sentinel;
    rbtree_insert_pt   insert;
};

#define rbtree_init(tree, s, i)			\
    (tree)->elem_size = 0;			\
    rbtree_sentinel_init(s);			\
    (tree)->root = s;				\
    (tree)->sentinel = s;			\
	  (tree)->insert = i


void rbtree_insert(rbtree_t *tree,
		   rbtree_node_t *node);
void rbtree_delete(rbtree_t *tree,
		   rbtree_node_t *node);
void rbtree_insert_value(rbtree_node_t *root, rbtree_node_t *node,
			 rbtree_node_t *sentinel);
void rbtree_insert_timer_value(rbtree_node_t *root,
			       rbtree_node_t *node, rbtree_node_t *sentinel);


#define rbt_red(node)               ((node)->color = 1)
#define rbt_black(node)             ((node)->color = 0)
#define rbt_is_red(node)            ((node)->color)
#define rbt_is_black(node)          (!rbt_is_red(node))
#define rbt_copy_color(n1, n2)      (n1->color = n2->color)


/* a sentinel must be black */

#define rbtree_sentinel_init(node)  rbt_black(node)

rbtree_node_t * rbtree_min(rbtree_node_t *node, rbtree_node_t *sentinel);
rbtree_node_t * rbtree_max(rbtree_node_t *node, rbtree_node_t *sentinel);

}


 
#endif /* _RBTREE_H_INCLUDED_ */
