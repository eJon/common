// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-12-18 09:13 
#include <gtest/gtest.h>
#include <cowbass4.hpp>
#include <map>
#include <st_timer.h>
#include <compare.hpp>

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

struct SampleNode {
    int k;
    int v;

    bool operator== (const SampleNode& other) const
    { return k==other.k && v == other.v; }
    
    void to_string (StringWriter& sw) const
    { sw << '(' << k << ',' << v << ')'; }
};

struct get_key {
    int operator() (const SampleNode& sn) const
    { return sn.k; }
};

TEST_F(test_usage_suite, random_insert_erase)
{
    srand(time(0));
    //srand(0);

    typedef std::map<int,int> ref_t;
    typedef Cowbass4<SampleNode,get_key> tr_t;

    ref_t refs[2];
    tr_t trs[2];
    ASSERT_EQ (0, trs[0].init());
    trs[1] = trs[0];
    
    SampleNode n;
    const SampleNode* p_n;

    cout << "sizeof(tr_t::const_iterator)=" << sizeof(tr_t::ConstIterator) << endl;
    
    const int MAX_VALUE = 10000;

    for (int j=0; j<10; ++j) {
        trs[1] = trs[0];
        refs[1] = refs[0];
        
        for (int i=0; i<MAX_VALUE; ++i) {
            n.k = rand()%MAX_VALUE;
            n.v = rand()%MAX_VALUE;
            int type = 0;
            if ((rand()%100) < 80) {
                p_n = trs[0].insert (n);
                refs[0][n.k] = n.v;
                ASSERT_TRUE (p_n);
                ASSERT_TRUE(*p_n == n);
            }
            else {
                type = 1;
                trs[0].erase (n.k);
                refs[0].erase (n.k);
            }
            //cout << "i=" << i << (type?" erase ":" insert ") << show(n).c_str() << " trs[0]=" << show(trs[0]).c_str() << endl;
            ASSERT_EQ (trs[0].size(), refs[0].size()) << (type?"erase ":"insert ") << show(n).c_str() << " trs[0]=" << show(trs[0]).c_str() << endl;
        }
    
        // for (int i=0; i<MAX_VALUE*2; ++i) {
        //     const int k = rand() % MAX_VALUE;
        //     trs[0].erase (k);
        //     refs[0].erase (k);
        //     cout << "i=" << i << " E" << " k=" << k << " trs[0]=" << show(trs[0]).c_str() << endl;
        //     ASSERT_EQ (trs[0].size(), refs[0].size()) << "k=" << k << " trs[0]=" << show(trs[0]).c_str() << endl;
        // }
    
        //cout << "trs[0]=" << show(trs[0].begin(), trs[0].end(), INT_MAX) << endl;
        //cout << "refs[0]=" << show(refs[0].begin(), refs[0].end(), INT_MAX) << endl;
        for (int k=0; k<2; ++k) {
            cout << "trs[" << (k==0?"NEW":"OLD") << "]=" << show(trs[k]).c_str() << endl;
            //cout << "trs[" << (k==0?"NEW":"OLD") << "].size=" << trs[k].size() << endl;
            ASSERT_EQ (refs[k].size(), trs[k].size());
            for (ref_t::const_iterator it=refs[k].begin(); it!=refs[k].end(); ++it) {
                //cout << "seek for " << it->first << "," << it->second << endl;
                p_n = trs[k].seek(it->first);
                ASSERT_TRUE (p_n) << "j=" << j << " k=" << k;
                ASSERT_EQ(p_n->v, it->second);
            }

            ref_t::const_iterator it1=refs[k].begin();
            tr_t::ConstIterator it2 = trs[k].begin(), it2_e = trs[k].end();
            size_t act_sz = 0;
            for (; it1!=refs[k].end() && it2!=it2_e; ++it1, ++it2, ++ act_sz) {
                ASSERT_EQ (it1->first, it2->k);
                ASSERT_EQ (it1->second, it2->v);
            }

            ASSERT_EQ (act_sz, refs[k].size());
        }
        cout << endl;
    }

    for (int i=0; i<2; ++i) {
        trs[i].clear();
    }
    
    ASSERT_EQ (trs[0].leaf_alloc()->alloc_num(), 0ul);
    ASSERT_EQ (trs[0].branch_alloc()->alloc_num(), 0ul);
}

#if 1
TEST_F(test_usage_suite, insert_consecutive_integer)
{
    srand(time(0));

    typedef std::map<int,int> ref_t;
    typedef Cowbass4<SampleNode,get_key> tr_t;
    const int MAX_VALUE = 10000;

    ref_t ref;
    tr_t tr;
    ASSERT_EQ (0, tr.init());
    std::vector<SampleNode> src;
    src.reserve (MAX_VALUE);
        
    SampleNode n;
    const SampleNode* p_n;
    NaiveTimer t;
    int s = 0;

    for (int j=0; j<3; ++j) {
        ref.clear();
        tr.clear();
        src.clear();
        
        if (j == 0) {
            cout << "** inserting ascent integers **" << endl;
            for (int i=0; i<MAX_VALUE; ++i) {
                n.k = i;
                n.v = rand() % MAX_VALUE;
                src.push_back (n);
            }
        }
        else if (j == 1) {
            cout << "** inserting descent integers **" << endl;
            for (int i=0; i<MAX_VALUE; ++i) {
                n.k = MAX_VALUE - i;
                n.v = rand() % MAX_VALUE;
                src.push_back (n);
            }
        }
        else {  // j==3
            cout << "** inserting randomized integers **" << endl;
            for (int i=0; i<MAX_VALUE; ++i) {
                n.k = rand() % MAX_VALUE;
                n.v = rand() % MAX_VALUE;
                src.push_back (n);
            }
        }
        
        {
            TIME_SCOPE("tr.insert", src.size());
            for (size_t i=0; i<src.size(); ++i) {
                p_n = tr.insert (src[i]);
            }
        }

        {
            TIME_SCOPE("ref.insert", src.size());
            for (size_t i=0; i<src.size(); ++i) {
                const SampleNode& e = src[i];
                ref[e.k] = e.v;
            }
        }

        {
            TIME_SCOPE_NO_NL("tr.traverse", tr.size());
            //t.start();
            s = 0;
            for (tr_t::ConstIterator it=tr.begin(), it_e=tr.end();
                 it!=it_e; ++it) {
                s += it->v;
            }
        }
        //t.stop();
        //cout << "tr.traverse=" << t.u_elapsed()*1000.0/src.size();
        cout << ", s=" << s << endl;

        {
            TIME_SCOPE_NO_NL("ref.traverse", ref.size());
            s = 0;
            for (ref_t::const_iterator it=ref.begin(), it_e=ref.end();
                 it!=it_e; ++it) {
                s += it->second;
            }
        }
        cout << ", s=" << s << endl;

        //cout << "tr=" << show(tr.begin(), tr.end(), INT_MAX) << endl;
        //cout << "ref=" << show(ref.begin(), ref.end(), INT_MAX) << endl;
        //cout << show (tr).c_str() << endl;
    
        {
            TIME_SCOPE_NO_NL("tr.seek", src.size());
            for (size_t i=0; i<src.size(); ++i) {
                tr.seek (src[i].k);
            }
        }
        cout << endl;

        {
            TIME_SCOPE_NO_NL("ref.seek", src.size());
            for (size_t i=0; i<src.size(); ++i) {
                ref.find (src[i].k);
            }
        }
        cout << endl;

        cout << "size=" << tr.size() << endl;
        ASSERT_EQ (ref.size(), tr.size());
        
        ref_t::const_iterator it1=ref.begin();
        tr_t::ConstIterator it2 = tr.begin(), it2_e = tr.end();
        size_t act_sz = 0;
        for (; it1!=ref.end() && it2!=it2_e; ++it1, ++it2, ++ act_sz) {
            ASSERT_EQ (it1->first, it2->k);
            ASSERT_EQ (it1->second, it2->v);
        }

        (void)p_n;
        ASSERT_EQ (act_sz, ref.size());
    }

    tr.clear();
    ASSERT_EQ (tr.leaf_alloc()->alloc_num(), 0ul);
    ASSERT_EQ (tr.branch_alloc()->alloc_num(), 0ul);
}

#endif

