// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-12-08 14:12 

#include <gtest/gtest.h>
#include "st_timer.h"
#include <algorithm>
#include <vector>
#include "get_proc_stat.h"
#include "unused/ext_hash_map.hpp"

using namespace st;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class test_general_perf_suite : public ::testing::Test{
protected:
    test_general_perf_suite(){};
    virtual ~test_general_perf_suite(){};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};

template <int _Size>
struct DummyObj {
    int id;
    int str[_Size];
};

template <>
struct DummyObj<0> {
    int id;
};

template <int _Size> inline void cause_minflt ()
{
    NaiveTimer t;
    const int N_ELEM = 100000;
    DummyObj<_Size>* d = new DummyObj<_Size>[N_ELEM];
    bool force_load = false;
    if (rand() & 1) {
        force_load = true;
        for (int i=0; i<N_ELEM; ++i) {
            d[i].id = rand();
        }
    }
    proc_stat_t ps1, ps2;
    get_current_proc_stat(&ps1);
    
    int s = 0;
    t.start();
    for (int i=0; i<N_ELEM; ++i) {
        s += d[i].id;
        //__builtin_prefetch(&(d[i+1].id), 0, 1);
    }
    t.stop();
    double e = t.u_elapsed()*1000.0 / N_ELEM;
    get_current_proc_stat(&ps2);
    cout << "sz=" << sizeof(DummyObj<_Size>)
         << " cyc=" << e*2.4
         << " sz/t=" << 1000000*e/sizeof(DummyObj<_Size>)
         << " " << force_load
         << " minflt=" << (ps2.minflt-ps1.minflt)
         << " vm_to_rss=" << (ps2.rss-ps1.rss)
         << " " << s << endl;

    delete [] d;
};

template <int _Times, int _Offset>
struct loop_cause_minflt {
    void operator () () const
    {
        cause_minflt<_Times+_Offset>();
        loop_cause_minflt<_Times-1, _Offset>()();
    }
};

template <int _Offset>
struct loop_cause_minflt<0,_Offset> {
    void operator () () const
    {
        cause_minflt<_Offset>();
    }    
};

#if 0
TEST_F(test_general_perf_suite, minor_page_fault)
{
    cout << "sizeof(int)=" << sizeof(int) << endl
         << "sizeof(long)=" << sizeof(long) << endl
        ;

    const int N_ELEM = 100000;
    int* a = new int[N_ELEM];
    long* b = new long[N_ELEM];
    int* c = (int*)b;
    for (int i=0; i<N_ELEM; ++i) {
        a[i] = rand();
        b[i] = ((long)rand()<<32) | rand();
    }

    NaiveTimer t;
    long s = 0;
    t.start();
    for (int i=0; i<N_ELEM; ++i) {
        s += a[i];
    }
    t.stop();
    cout << "s=" << s
         << " t1=" << t.u_elapsed()*1000.0 / N_ELEM << " ns"
         << endl;

    s = 0;
    t.start();
    for (int i=0; i<N_ELEM; ++i) {
        s += b[i];
    }
    t.stop();
    cout << "s=" << s
         << " t2=" << t.u_elapsed()*1000.0 / N_ELEM << " ns"
         << endl;

    const int N_ELEM2 = (N_ELEM << 1);
    s = 0;
    t.start();
    for (int i=0; i<N_ELEM2; ++i) {
        s += c[i];
    }
    t.stop();
    cout << "s=" << s
         << " t3=" << t.u_elapsed()*1000.0 / N_ELEM << " ns"
         << endl;
    
    loop_cause_minflt<100,300>()();
}
#endif

#if 0
TEST_F(test_general_perf_suite, cache_speed)
{
    const size_t MAX_LEN = (1 << 29);
    char* p_mem = new char[MAX_LEN];
    memset (p_mem, 0x12, MAX_LEN);
    size_t cap = (1 << 10);
    const int REP = 1000000;
    NaiveTimer t;

    t.start();
    int s = 0;
    for (int i=0; i<REP; ++i) {
        s += rand();
    }
    t.stop();
    double rand_cost = t.u_elapsed()*1000.0/REP;

    cout << "mem_size\tt" << endl;

    for (; cap <= MAX_LEN; cap = (size_t)(cap*1.414)) {
        t.start();
        for (int i=0; i<REP; ++i) {
            ++ p_mem[((u_int)rand()) % cap];
        }
        t.stop();
        cout << cap << "\t" << t.u_elapsed()*1000.0/REP << endl;
    }

    delete [] p_mem;
}
#endif


#if 0
struct NodeKey {
    u_int data;
    u_int next;

    bool operator< (const NodeKey& other) const
    { return next < other.next; }
};

template <int _Times>
struct add_up_to {
    template <typename _T>
    inline static _T run (_T a[])
    { return *a + add_up_to<_Times-1>::run(a+1); }
};

template <>
struct add_up_to<1> {
    template <typename _T>
    inline static _T run (_T a[])
    { return *a; }
};


const int NODE_VALUE_NUM = 6;
struct NodeValue {
    u_int data[NODE_VALUE_NUM];
};

struct NodeAll {
    NodeKey key;
    NodeValue value;
};

inline bool assign_value(u_int& target, u_int src)
{
    target = src;
    return true;
}

TEST_F(test_general_perf_suite, list_forward)
{
    srand(time(0));
    
    const int LEN = 1000000;
    const int REP = 10000000;
    NodeAll* a_node = new NodeAll[LEN];

    NodeKey* a_key = new NodeKey[LEN];
    NodeValue* a_value = new NodeValue[LEN];

    for (int i=0; i<LEN; ++i) {
        a_key[i].next = (i<LEN-1) ? (i+1) : 0;
        a_key[i].data = rand();
        for (int j=0; j<NODE_VALUE_NUM; ++j) {
            a_value[i].data[j] = rand();
        }
    }
    //std::random_shuffle (a_key, a_key+LEN);

    for (int i=0; i<LEN; ++i) {
        a_node[i].key = a_key[i];
        a_node[i].value = a_value[i];
    }

    NaiveTimer t;
    u_int s;
    
    t.start();
    s = 0;
    u_int mask = (1<<1)-1;
    for (int i=0,p=i; i<REP; ++i) {
        if ((a_node[p].key.data+i) & mask) {
            for (int j=0; j<NODE_VALUE_NUM; ++j) {
                s += a_node[p].value.data[j];
            }
            //s += add_up_to<NODE_VALUE_NUM>::run(a_node[p].value.data);
        }
        p = a_node[p].key.next;
    }
    t.stop();
    cout << "t=" << t.u_elapsed()*1000.0/REP << " " << s << endl;

    t.start();
    s = 0;
    int c1=0, c2=0;
    int i=0,p=i;
    while (__builtin_expect(i<REP, 1)) {
        s += !!((a_key[p].data+i) & mask)
            * add_up_to<NODE_VALUE_NUM>::run(a_value[p].data);
        p = a_key[p].next;
        ++i;
    }
    t.stop();
    cout << "t=" << t.u_elapsed()*1000.0/REP
         << " " << s << " " << c1 << " " << c2 << endl;
}
#endif


template <typename _Iterator, typename _Value=typename _Iterator::value_type>
struct MergeIterator {
    typedef MergeIterator<_Iterator,_Value> Self;

    MergeIterator (const _Iterator& it1
                   , const _Iterator& it2
        )
    {
        a_it_[0] = it1;
        a_it_[1] = it2;
        b_ = (*a_it_[0] > *a_it_[1]);
    }

    _Value& operator* () const
    { return *a_it_[b_]; }

    bool operator!= (const Self& other) const
    //{ return it1_!=other.it1_ && it2_!=other.it2_; }
    { return a_it_[b_] != other.a_it_[other.b_]; }

    inline void operator++ ()
    {
        a_it_[0] += (!b_);
        a_it_[1] += b_;
        //++ a_it_[b_];
        b_ = (*a_it_[0] > *a_it_[1]);
        
        // if (it1_ != it1_e_) {
        //     if (it2_ != it2_e_) {
        //         if (*it1_ < *it2_) {
        //             p_value_ = &(*it1_);
        //             ++ it1_;
        //         }
        //         else {
        //             p_value_ = &(*it2_);
        //             ++ it2_;
        //         }
        //     }
        //     else {
        //         p_value_ = &(*it1_);
        //         ++ it1_;
        //     }
        // }
        // else {
        //     bool b = (it2_ != it2_e_);
        //     p_value_ = (_Value*)(b * (intptr_t)&(*it2_));
        //     b && (++ it2_);
        // }

        // register bool b1 = (it1_ != it1_e_);
        // register bool b2 = (it2_ != it2_e_);
        // register bool b3 = b1 && b2 && (*it1_<*it2_);
        // register bool b4 = (b1 && (!b2 || b3));
        // register bool b5 = (b2 && !(b1 && b3));
        // p_value_ = (_Value*)(b4 * (intptr_t)(&*it1_) + b5 * (intptr_t)(&*it2_));
        // c += b1 + b2;
        //  //b4 ? (++it1_) : (b5 ? (++it2_) :
        // it1_ += b4;
        // it2_ += b5;
    }
    
    int b_;
    _Iterator a_it_[2];
};

TEST_F(test_general_perf_suite, merge_list)
{
    NaiveTimer t;

    const int LEN = 1000000;
    int* a = new int[LEN+1];
    int* b = new int[LEN+1];
    for (int i=0; i<LEN; ++i) {
        a[i] = rand();
        b[i] = rand();
    }
    a[LEN] = INT_MAX;
    b[LEN] = INT_MAX;
    std::sort (a, a+LEN);
    std::sort (b, b+LEN);

    int s = 0;
    s = 0;
    t.start ();
    for (int i=0; i<LEN; ++i) {
        bool d = a[i] < b[i];
        s += (d^1) * a[i] + b[i];
    }
    t.stop ();
    cout << "t=" << t.u_elapsed()*1000.0/LEN << " " << s << endl;

    int n = 0;
    t.start ();
    for (MergeIterator<int*,int> mi(a,b),mi_e(a+LEN,b+LEN); mi!=mi_e; ++ mi, ++n) {
        s += *mi;
        //cout << " " << *mi;
    }
    t.stop();
    cout << endl << "t=" << t.u_elapsed()*1000.0/n << " " << s << endl;

    delete[] a;
    delete[] b;
}



struct BHI {
    virtual ~BHI() {}
    virtual int* seek (int key) const = 0;
    virtual void insert (int key, int value) = 0;
    int type;
};

struct TrivialHashIndex1 : public BHI {
    TrivialHashIndex1()
    {
        this->type = 1;
    }
    
    int* seek (int key) const
    {
        return hm_.seek (key);
    }
    int* seek_non_virt (int key) const
    {
        return hm_.seek (key);
    }
    void insert (int key, int value)
    {
        hm_.insert (key, value);
    }

    ExtHashMap<int,int> hm_;
};

struct TrivialHashIndex2 : public BHI {
    TrivialHashIndex2()
    {
        this->type = 2;
    }
    
    int* seek (int key) const
    {
        return hm_.seek (key);
    }
    int* seek_non_virt (int key) const
    {
        return hm_.seek (key);
    }
    void insert (int key, int value)
    {
        hm_.insert (key, value);
    }


    ExtHashMap<int,int> hm_;
};


TEST_F(test_general_perf_suite, test_virtual_seek)
{
    TrivialHashIndex1 thi;
    BHI* p_thi = &thi;
    std::vector<int> keys;
    const int MAX_VALUE = 1000000;
    for (int i=0; i<MAX_VALUE; ++i) {
        int k = rand()%MAX_VALUE;
        keys.push_back (k);
        p_thi->insert (k, rand()%MAX_VALUE);
    }
    std::sort (keys.begin(), keys.end());
    std::vector<int>::iterator new_end = std::unique (keys.begin(), keys.end());
    keys.resize (new_end - keys.begin());
    
    NaiveTimer t;
    t.start();
    for (size_t i=0; i<keys.size(); ++i) {
        p_thi->seek (keys[i]);
    }
    t.stop();
    cout << "virt.t=" << t.u_elapsed()*1000.0/keys.size() << "ns"
         << endl;

    t.start();
    for (size_t i=0; i<keys.size(); ++i) {
        thi.seek_non_virt (keys[i]);
    }
    t.stop();
    cout << "non_virt.t=" << t.u_elapsed()*1000.0/keys.size() << "ns"
         << endl;

    t.start();
    for (size_t i=0; i<keys.size(); ++i) {
        switch (p_thi->type) {
        case 1:
            ((TrivialHashIndex1*)p_thi)->seek_non_virt (keys[i]);
            break;
        case 2:
            ((TrivialHashIndex2*)p_thi)->seek_non_virt (keys[i]);
            break;
        }
    }
    t.stop();
    cout << "virt_inline.t=" << t.u_elapsed()*1000.0/keys.size() << "ns"
         << endl;
}




struct SwitchDummy
{
    SwitchDummy(int opt) : opt_(opt) {}
    
    int operator() (const int rep) const
    {
        int c = 2;
        for (int i=0; i<rep; ++i) {
            switch (opt_) {
            case 0:
                c += 1;
                break;
            case 1:
                c += c*c*c*c*c*c*c*c+1;
                break;
            case 2:
                c += c*c*c*c*c*c*c*c*c*c*c*c*c*c*c*c*c*c+2;
                break;
            }
        }
        return c;
    }

    int opt_;
};

struct ConstSwitchDummy {
    ConstSwitchDummy(const int opt) : opt_(opt) {}
    
    int operator() (const int rep) const
    {
        int c = 2;
        for (int i=0; i<rep; ++i) {
            switch (opt_) {
            case 0:
                c += 1;
                break;
            case 1:
                c += c*c*c*c*c*c*c*c+1;
                break;
            case 2:
                c += c*c*c*c*c*c*c*c*c*c*c*c*c*c*c*c*c*c+2;
                break;
            }
        }
        return c;
    }

    int optimized (const int rep) const
    {
        static const void* const labels[] = { &&CASE0, &&CASE1, &&CASE2 };
        __builtin_prefetch (labels[opt_], 0, 3);

        int c = 2;
        for (int i=0; i<rep; ++i) {
            goto *labels[opt_];
        CASE0:
            c += 1;
            continue;
        CASE1:
            c += c*c*c*c*c*c*c*c+1;
            continue;
        CASE2:
            c += c*c*c*c*c*c*c*c*c*c*c*c*c*c*c*c*c*c+2;
            continue;
        }
        return c;
    }

    const int opt_;
};



TEST_F(test_general_perf_suite, test_switch_const)
{
    NaiveTimer t;
    int opt=0;
    if (rand() < 10) {
        cout << "changing opt at here\n";
        opt = 2;
    }
    const int rep = 10000000;

    SwitchDummy sd(opt);
    t.start();
    int c = sd(rep);
    t.stop();
    cout << "c=" << c << ",t1=" << t.u_elapsed()*1000.0/rep << "ns" << endl;

    ConstSwitchDummy sd2(opt);
    t.start();
    c = sd2.optimized(rep);
    t.stop();
    cout << "c=" << c << ",t1'=" << t.u_elapsed()*1000.0/rep << "ns" << endl;
    
    t.start();
    c = 0;
    for (int i=0; i<rep; ++i) {
        c += 1;
    }
    t.stop();
    cout << "c=" << c << ",t2=" << t.u_elapsed()*1000.0/rep << "ns" << endl;
    
}

