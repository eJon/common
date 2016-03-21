#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
using namespace std;

#define POOLSIZE 100 
#define LISTEND -1

// Test some simple behavior of a list connected by
// offsets instead of pointers.
// The list still looks the same from the outside.

typedef long Offset;

// The beginning address of memory where nodes resides.
// This can only be changed by MemPool.
char* base = 0;

// Basic memory pool using mmap to get memory.
// Can't do recycling work.
struct MemPool {
    MemPool()
        : mem_base(NULL)
        , pos(0)
        , fd(-1)
        , pool_size(POOLSIZE)
        , has_remapped(false)
    {}

    void reset ()
    {
        pos = 0;
    }

    int init()
    { 
        fd = open ("./tmp", O_RDWR | O_CREAT | O_TRUNC, 
                   S_IRUSR | S_IWUSR);
        if (fd < 0) {
            cout << "Can't open file ./tmp" << endl;
            return -1;
        }

        if (lseek (fd, pool_size - 1, SEEK_SET) < 0) {
            cout << "Can't lseek" << endl;
            return -1;
        }

        if (write (fd, "", 1) != 1) {
            cout << "Can't write into ./tmp" << endl;
            return -1;
        }

        mem_base = static_cast<char*>
            (mmap (0, pool_size, PROT_READ | PROT_WRITE, 
                   MAP_SHARED, fd, 0));
        if (mem_base == MAP_FAILED) {
            cout << "mmap failed" << endl;
            return -1;
        }

        base = mem_base;
        return 0;
    }
    
    ~MemPool()
    { close (fd); }

    void* alloc_object (int obj_size)
    {
        if (pos + obj_size <= pool_size) {
            pos += obj_size;
            return mem_base + pos - obj_size;
        } else {
            if (!remap()) {
                cout << "Can't alloc anymore." << endl;
                return NULL;
            } else {
                pos += obj_size;
                return mem_base + pos - obj_size;
            } 
        }
    }

    // Calling mremap to enlarge memory pool.
    // Must increase file size first.
    bool remap()
    {
        //cout << "remap called" << endl;
        if (lseek (fd, POOLSIZE - 1, SEEK_END) < 0) {
            cout << "Can't lseek" << endl;
            return false;
        } 
        
        if (write (fd, "", 1) != 1) {
            cout << "Can't write into ./tmp" << endl;
            return false;
        }

        char* new_base = static_cast<char*>
            (mremap (mem_base, pool_size, pool_size + POOLSIZE,
                     MAP_SHARED));
        if (new_base == MAP_FAILED) {
            cout << "Can't remap" << endl;
            return false;
        } else {
            pool_size += POOLSIZE;
            base = mem_base = new_base;
            has_remapped = true;
            return true;
        }
    }
    
    char* mem_base;
    int pos;
    int fd;
    int pool_size;
    bool has_remapped;
};

struct Node {
    int value;
    Offset off;
};

struct List {
    List(): head(LISTEND), length(0) 
    {}

    int init() { return pool.init(); }

    void reset () 
    {
        head = LISTEND;
        length = 0;
        pool.reset();
    }
    
    Node* insert_before (int v)
    {
        Node* p_node = static_cast<Node*>
            (pool.alloc_object (sizeof (Node)));
        if (p_node != NULL) {
            p_node->value = v;
            if (length > 0) {
                p_node->off = head;
            } else {
                p_node->off = LISTEND;
            }
            head = reinterpret_cast<char*>(p_node) - 
                base;
            length++;
        }
        return p_node;
    }

    struct Iterator {
        explicit Iterator (Offset head): cur_off(head)
        {}

        Node* operator->()
        { 
            if (cur_off >= 0) {
                return reinterpret_cast<Node*>
                    (base + cur_off); 
            } else {
                return NULL;
            }
        }
            
        void operator++()
        { 
            Node* p_node = this->operator->();
            if (p_node != NULL) {
                cur_off = p_node->off;    
            }
        }

        bool operator== (Iterator& rhs)
        { return cur_off == rhs.cur_off; }

        bool operator!= (Iterator& rhs)
        { return !(this->operator== (rhs)); }
        
        Offset cur_off;
    };

    Iterator begin() { return Iterator (head); }

    static Iterator end() { return Iterator (LISTEND); }
    
    MemPool pool;
    Offset head;
    int length;
};

struct StdNode {
    int value;
    StdNode* next;
};

struct StdPool {
    StdPool()
        : mem_base(NULL)
        , pos(0)
        , pool_size(POOLSIZE)
    {}

    void reset ()
    {
        pos = 0;
    }

    int init()
    {
        mem_base = new (std::nothrow) char[pool_size];
        if (mem_base == NULL) {
            cout << "Out of memory in StdPool" << endl;
            return -1;
        }
        return 0;
    }


    ~StdPool()
    { delete [] mem_base; }

    void* alloc_object (int obj_size)
    {
        if (pos + obj_size <= pool_size) {
            pos += obj_size;
            return mem_base + pos - obj_size;
        } else {
            if (!resize()) {
                cout << "Can't alloc anymore." << endl;
                return NULL;
            } else {
                pos += obj_size;
                return mem_base + pos - obj_size;
            } 
        }
    }

    bool resize()
    {
        //cout << "resize called" << endl;

        void* new_base = realloc 
            (mem_base, pool_size + POOLSIZE);
        if (new_base == NULL) {
            cout << "Can't resize" << endl;
            return false;
        } else {
            pool_size += POOLSIZE;
            mem_base = static_cast<char*>(new_base);
            return true;
        }
    }
        
    char* mem_base;
    int pos;
    int pool_size;
};

struct StdList {
    StdList(): head(NULL), length(0) 
    {}

    int init() { return pool.init(); }

    void reset ()
    {
        head = NULL;
        length = 0;
        pool.reset();
    }
    
    StdNode* insert_before (int v)
    {
        StdNode* p_node = static_cast<StdNode*>
            (pool.alloc_object (sizeof (StdNode)));
        if (p_node != NULL) {
            p_node->value = v;
            if (length > 0) {
                p_node->next = head;
            } else {
                p_node->next = NULL;
            }
            head = p_node;
            length++;
        }
        return p_node;
    }

    struct Iterator {
        explicit Iterator (StdNode* head): cur_node(head)
        {}

        StdNode* operator->() { return cur_node; }
            
        void operator++()
        { 
            if (cur_node != NULL) {
                cur_node = cur_node->next;    
            }
        }

        bool operator== (Iterator& rhs)
        { return cur_node == rhs.cur_node; }

        bool operator!= (Iterator& rhs)
        { return !(this->operator== (rhs)); }
        
        StdNode* cur_node;
    };

    Iterator begin() { return Iterator (head); }

    static Iterator end() { return Iterator (NULL); }
    
    StdPool pool;
    StdNode* head;
    int length;
};

int main()
{
    const int NUM = 1000000;
    List list;
    StdList std_list;
    timeval start;
    timeval end;
    
    if (list.init() || std_list.init()) {
        cout << "List initialization failed" << endl;
        return -1;
    }
    
    gettimeofday (&start, NULL);
    for (int i = 0; i < NUM; i++) {
        list.insert_before (i);
    }
    gettimeofday (&end, NULL);
    cout << "Insert gettimeofday for mremap version " 
        << (end.tv_usec - start.tv_usec) << "us"<< endl;
     
    gettimeofday (&start, NULL);
    for (int i = 0; i < NUM; i++) {
        std_list.insert_before (i);
    }
    gettimeofday (&end, NULL);
    cout << "Insert time for realloc version " 
        << (end.tv_usec - start.tv_usec) << "us"<< endl;

    list.reset();
    std_list.reset();
    
    gettimeofday (&start, NULL);
    for (int i = 0; i < NUM; i++) {
        list.insert_before (i);
    }
    gettimeofday (&end, NULL);
    cout << "(reset) Insert gettimeofday for mremap version " 
        << (end.tv_usec - start.tv_usec) << "us"<< endl;
    
    gettimeofday (&start, NULL);
    for (int i = 0; i < NUM; i++) {
        std_list.insert_before (i);
    }
    gettimeofday (&end, NULL);
    cout << "(reset) Insert time for realloc version " 
        << (end.tv_usec - start.tv_usec) << "us"<< endl;
    
    for (List::Iterator it = list.begin(), it_e = list.end();
            it != it_e; ++it) {
        //cout << it->value << " ";
    }
    cout << endl;
    
    for (StdList::Iterator it = std_list.begin(), 
            it_e = std_list.end(); it != it_e; ++it) {
        //cout << it->value << " ";
    }
    cout << endl;
    
    return 0;
}
