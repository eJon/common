// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/os/mempool.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <string.h>
#include "os/memalloc.h"
#include "os/mempool.h"

NSPIO_DECLARATION_START


#define align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define align_ptr(p, a)							\
    (int8_t *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))


static void *palloc_block(mempool_t *pool, uint32_t size);
static void *palloc_large(mempool_t *pool, uint32_t size);

#define max(a, b) ((a) > (b) ? (a) : (b))

mempool_t *mp_create(uint32_t size) {
    mempool_t *p = NULL;

    if (!(size = max(size, SLB_PAGESIZE)))
        return NULL;
    if (!(p = (mempool_t *) mem_align(POOL_ALIGNMENT, size)))
        return NULL;
    p->d.last = (int8_t *) p + sizeof(mempool_t);
    p->d.end = (int8_t *) p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(mempool_t);
    p->max = (size < MAX_ALLOC_FROM_POOL) ? size : MAX_ALLOC_FROM_POOL;

    p->current = p;
    p->large = NULL;
    p->cleanup = NULL;

    return p;
}


void mp_destroy(mempool_t *pool) {
    mempool_t *p, *n;
    mempool_large_t    *l;
    mempool_cleanup_t  *c;
    
    for (c = pool->cleanup; c; c = c->next) {
        if (c->handler)
            c->handler(c->data);
    }

    for (l = pool->large; l; l = l->next) {
        if (l->alloc)
            free(l->alloc);
    }

#if (DEBUG)

    /*
     * we could allocate the pool->log from this pool
     * so we cannot use this log while free()ing the pool
     */

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        if (n == NULL) {
            break;
        }
    }

#endif

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        free(p);

        if (n == NULL) {
            break;
        }
    }
}


void mp_reset(mempool_t *pool) {
    mempool_t *p;
    mempool_large_t  *l;
    
    for (l = pool->large; l; l = l->next) {
        if (l->alloc)
            free(l->alloc);
    }
    pool->large = NULL;
    for (p = pool; p; p = p->d.next) {
        p->d.last = (int8_t *) p + sizeof(mempool_t);
    }
}


void *mp_alloc(mempool_t *pool, uint32_t size) {
    int8_t      *m = NULL;
    mempool_t  *p = NULL;
    void *ptr = NULL;
    
    if (size <= pool->max) {
        p = pool->current;
        do {
            m = align_ptr(p->d.last, POOL_ALIGNMENT);
            if ((uint32_t) (p->d.end - m) >= size) {
                p->d.last = m + size;
                return m;
            }
            p = p->d.next;
        } while (p);
        return palloc_block(pool, size);
    }

    ptr = palloc_large(pool, size);

    return ptr;
}


void *mp_nalloc(mempool_t *pool, uint32_t size) {
    int8_t      *m = NULL;
    mempool_t  *p = NULL;
    void *ptr = NULL;

    if (size <= pool->max) {
        p = pool->current;
        do {
            m = p->d.last;
            if ((uint32_t) (p->d.end - m) >= size) {
                p->d.last = m + size;
                return m;
            }
            p = p->d.next;
        } while (p);
        return palloc_block(pool, size);
    }

    ptr = palloc_large(pool, size);
    return ptr;
}


static void *palloc_block(mempool_t *pool, uint32_t size)
{
    int8_t      *m;
    uint32_t       psize;
    mempool_t  *p, *current, *new_alloc;

    psize = (uint32_t) (pool->d.end - (int8_t *) pool);

    m = (int8_t *)mem_align(POOL_ALIGNMENT, psize);
    if (m == NULL) {
        return NULL;
    }

    new_alloc = (mempool_t *) m;

    new_alloc->d.end = m + psize;
    new_alloc->d.next = NULL;
    new_alloc->d.failed = 0;

    m += sizeof(mempool_data_t);
    m = align_ptr(m, POOL_ALIGNMENT);
    new_alloc->d.last = m + size;

    current = pool->current;

    for (p = current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            current = p->d.next;
        }
    }

    p->d.next = new_alloc;

    pool->current = current ? current : new_alloc;

    return m;
}


static void *palloc_large(mempool_t *pool, uint32_t size)
{
    void              *p;
    uint32_t         n;
    mempool_large_t  *large;

    p = mem_alloc(size);
    if (p == NULL) {
        return NULL;
    }

    n = 0;

    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }

    large = (mempool_large_t *)mp_alloc(pool, sizeof(mempool_large_t));
    if (large == NULL) {
        free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


void *mp_align(mempool_t *pool, uint32_t size, uint32_t alignment) {
    void              *p;
    mempool_large_t  *large;
    
    p = mem_align(alignment, size);
    if (p == NULL) {
        return NULL;
    }

    large = (mempool_large_t *)mp_alloc(pool, sizeof(mempool_large_t));
    if (large == NULL) {
        free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


int mp_free(mempool_t *pool, void *p) {
    mempool_large_t  *l;
    
    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            free(l->alloc);
            l->alloc = NULL;

            return 0;
        }
    }

    return -1;
}


void *mp_calloc(mempool_t *pool, uint32_t size) {
    void *p;
    
    p = mp_alloc(pool, size);
    if (p) {
        memset(p, 0, size);
    }

    return p;
}


mempool_cleanup_t *mempool_cleanup_add(mempool_t *p, uint32_t size) {
    mempool_cleanup_t  *c;
    
    c = (mempool_cleanup_t *)mp_alloc(p, sizeof(mempool_cleanup_t));
    if (c == NULL) {
        return NULL;
    }

    if (size) {
        c->data = mp_alloc(p, size);
        if (c->data == NULL) {
            return NULL;
        }

    } else {
        c->data = NULL;
    }

    c->handler = NULL;
    c->next = p->cleanup;

    p->cleanup = c;

    return c;
}


}
