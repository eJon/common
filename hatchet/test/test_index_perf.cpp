// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
/**
 * Performance testing for cowbass and tb_hashmap
 */

#include <gtest/gtest.h>

#include <vector>

#include "../st_timer.h"
#include <bd_inc_builder.h>
#include <index_op_template.h>
#include <bs_afs_if.h>
#include <afs_inc_if.h>
#include <memory_pool.h>
#include <sf_list2.h>
#include <data_manager.h>

#include "cowbass.hpp"
#include "tb_hashmap.hpp"
#include "functional.hpp"


template<> memory_pool_t bd_adqidx_list_t::_mp_list(sizeof(bd_adqidx_list_t));
template<> memory_pool_t bd_adqidx_list_t::_mp_node(sizeof(bd_adqidx_list_t::list_node_t));
template<> afs::list_setting_t<BS_AFS_LIB::site_adq_t> *bd_adqidx_list_t::_sort_setting = NULL;

template<> memory_pool_t ididx_list_t::_mp_list(sizeof(ididx_list_t));
template<> memory_pool_t ididx_list_t::_mp_node(sizeof(ididx_list_t::list_node_t));
template<> afs::list_setting_t<BS_AFS_LIB::index_unitlist_iterm_t> *ididx_list_t::_sort_setting = NULL;

template<> memory_pool_t bd_ididx_list_t::_mp_list(sizeof(bd_ididx_list_t));
template<> memory_pool_t bd_ididx_list_t::_mp_node(sizeof(bd_ididx_list_t::list_node_t));
template<> afs::list_setting_t<BS_AFS_LIB::bd_unitlist_iterm_t> *bd_ididx_list_t::_sort_setting = NULL;

template<> memory_pool_t fc_termunit_list_t::_mp_list(sizeof(fc_termunit_list_t));
template<> memory_pool_t fc_termunit_list_t::_mp_node(sizeof(fc_termunit_list_t::list_node_t));
template<> afs::list_setting_t<BS_AFS_LIB::fc_inv_unit_t> *fc_termunit_list_t::_sort_setting = NULL;

template<> memory_pool_t bd_termunit_list_t::_mp_list(sizeof(bd_termunit_list_t));
template<> memory_pool_t bd_termunit_list_t::_mp_node(sizeof(bd_termunit_list_t::list_node_t));
template<> afs::list_setting_t<BS_AFS_LIB::bd_inv_unit_t> *bd_termunit_list_t::_sort_setting = NULL;


template<> memory_pool_t unitad_list_t::_mp_list(sizeof(unitad_list_t));
template<> memory_pool_t unitad_list_t::_mp_node(sizeof(unitad_list_t::list_node_t));
template<> afs::list_setting_t<BS_AFS_LIB::unitdesc_idx_t> *unitad_list_t::_sort_setting = NULL;

template<> memory_pool_t unitkw_list_t::_mp_list(sizeof(unitkw_list_t));
template<> memory_pool_t unitkw_list_t::_mp_node(sizeof(unitkw_list_t::list_node_t));
template<> afs::list_setting_t<BS_AFS_LIB::kwid_iterm_t> *unitkw_list_t::_sort_setting = NULL;

template<> memory_pool_t unitterm_list_t::_mp_list(sizeof(unitterm_list_t));
template<> memory_pool_t unitterm_list_t::_mp_node(sizeof(unitterm_list_t::list_node_t));
template<> afs::list_setting_t<BS_AFS_LIB::unit_term_t> *unitterm_list_t::_sort_setting = NULL;

const int TOTAL_COUNT = 100000;
const int UNIT_COUNT = 5000;
const int MAX_LIST_LEN = 10000;

int main(int argc, char **argv)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


namespace afs
{
extern int list_unitad_bd_compare_cq(const BS_AFS_LIB::unitdesc_idx_t *node_a, const BS_AFS_LIB::unitdesc_idx_t *node_b, void *param);
}
namespace st
{
    template <>
    struct compare_f<BS_AFS_LIB::unitdesc_idx_t>
    {
        inline int operator() (const BS_AFS_LIB::unitdesc_idx_t key1, const BS_AFS_LIB::unitdesc_idx_t key2) const
        {
            return key1.idea_id - key2.idea_id;
        }
    };

    StringWriter& operator<< (StringWriter& sb, const BS_AFS_LIB::unitdesc_idx_t& d)
    {
        sb << "{idea_id=" << d.idea_id
           << ",wuliao_flag=" << d.wuliao_flag
           << ",player=" << d.player
           << ",height=" << d.height
           << ",width=" << d.width
           << "show_url_sign=" << d.show_url_sign[0] << "+" << d.show_url_sign[1]
           << "f_cate=" << d.f_cate
           << "s_cate=" << d.s_cate
           << "di_id=" << d.di_id;
        return sb;
    }

}

afs::list_setting_t<BS_AFS_LIB::unitdesc_idx_t> list_setting_unitad_bd = {afs::list_unitad_bd_compare_cq, NULL};

typedef st::cowbass_t<BS_AFS_LIB::unitdesc_idx_t> unitad_list_cowbass;
typedef st::tb_hashmap_t<int, unitad_list_cowbass*> unitad_dict_tb;

/**
// Brief: 
 **/
class test_perf_suite : public ::testing::Test{
protected:
    test_perf_suite():add_unit_idea_dict(BS_AFS_LIB::FC_UNIT_HASHSIZE, 10240), 
                         del_unit_idea_dict(BS_AFS_LIB::FC_UNIT_HASHSIZE, 10240),
                         unitad_dict(BS_AFS_LIB::FC_UNIT_HASHSIZE, BS_AFS_LIB::FC_BLOCK_SIZE),
                         mp(sizeof(unitad_list_cowbass::node_t))
    {
        unitad_dict_tb::create(ht, ht2, BS_AFS_LIB::FC_UNIT_HASHSIZE);
    };
    virtual ~test_perf_suite(){};
    virtual void SetUp() {
        prepare_data();
    };
    virtual void TearDown() {
        add_unit_idea_dict.clear();
        del_unit_idea_dict.clear();
        unitad_dict.clear();
        ht.clear();
        ht2.clear();
    };

    void prepare_data()
    {
        srand(time(NULL));

        for (int i = 0; i < UNIT_COUNT; ++i)
        {
            int unitid = i;
            BS_AFS_LIB::cpro_sign_t snode;
            snode.sign1 = unitid;
            snode.sign2 = BS_AFS_LIB::BD_DEFAULT_SIGN2;
        
            unitad_list_t *list = NULL;
            unitad_list_cowbass list2_tmp(&mp);
            unitad_list_cowbass *list2 = new unitad_list_cowbass(&mp);
            list2->create(&mp);

            list = unitad_list_t::create(&afs::list_setting_unitad_bd, true);
            for(int m = 0; m < MAX_LIST_LEN; m++)
            {
                BS_AFS_LIB::unitdesc_idx_t idea;
                idea.idea_id = m;
                idea.wuliao_flag = BS_AFS_LIB::TXT_TYPE;
                idea.player = rand()%15;
                idea.height = rand()%768;
                idea.width = rand()%1024;
                idea.di_id = rand()%100;
                list->insert(&idea);
                list2_tmp.add(idea);
            }
            list->sort();

            //FIXME, following line is commented to enable compiling
            //list2_tmp.spawn(list2);
            unitad_dict.insert(snode, list, false);
            ht.set(unitid, list2);
        }
        ht.spawn(ht2);
        ht2.spawn(ht);
    }

    BS_AFS_LIB::unitad_dict_t add_unit_idea_dict;
    BS_AFS_LIB::unitad_dict_t del_unit_idea_dict;
    BS_AFS_LIB::unitad_dict_t unitad_dict;

    unitad_dict_tb ht;
    unitad_dict_tb ht2;
    st::allocator_t<MP_ID> mp;

};


/**
// Brief:  
 * @begin_version 
 **/
TEST_F(test_perf_suite, merge)
{
    naive_timer_t timer;
    int list_count = 0;

    ulong addtime = 0;
    ulong addtime2 = 0;

    for (int i = 0; i < TOTAL_COUNT; ++i)
    {
        int unitid = rand() % UNIT_COUNT;

        BS_AFS_LIB::cpro_sign_t snode;
        snode.sign1 = unitid;
        snode.sign2 = BS_AFS_LIB::BD_DEFAULT_SIGN2;
        
        afs::bd_event_header_t event_head;
        event_head.event_id = i;
        event_head.level = afs::LEVEL_IDEA;
        event_head.type = i % 2?TYPE_OP_ADD:TYPE_OP_DEL;
        
        BS_AFS_LIB::unitdesc_idx_t idea;
        idea.idea_id = rand() % UNIT_COUNT;
        idea.wuliao_flag = BS_AFS_LIB::TXT_TYPE;
        idea.player = rand()%15;
        idea.height = rand()%768;
        idea.width = rand()%1024;
        idea.di_id = rand()%100;

        timer.start();
        handler_index_event(&del_unit_idea_dict, 
                            &add_unit_idea_dict, 
                            snode, idea, &list_setting_unitad_bd, event_head);
        timer.stop();
        addtime += timer.u_elapsed();

        unitad_list_cowbass *list_tmp = NULL;
        timer.start();
        if (TYPE_OP_ADD == event_head.type)
        {
            unitad_dict_tb::iterator it;
            if ((it = ht2.find(unitid)) == ht2.end())
            {
                list_tmp = new unitad_list_cowbass(&mp);
            }
            else
            {
                list_tmp = it->second();
                list_tmp->add(idea);
            }
        }
        else if (TYPE_OP_DEL == event_head.type)
        {
            unitad_dict_tb::iterator it;
            if ((it = ht2.find(unitid)) != ht2.end())
            {
                list_tmp = it->second();
                list_tmp->del(idea);
            }
        }
        ht2.set(unitid, list_tmp);

        timer.stop();
        addtime2 += timer.u_elapsed();
    }

    printf("hashtemplate & sortedlist addtime:%lu us\n", addtime);
    printf("tb_hashmap & cowbass addtime:%lu us\n", addtime2);

    //hash template
    printf("unitad_dict key size:%d, value size:%d\n", get_key_num(&unitad_dict), get_value_num(&unitad_dict));
    printf("add_unit_idea_dict key size:%d, value size:%d\n", get_key_num(&add_unit_idea_dict), get_value_num(&add_unit_idea_dict));
    printf("del_unit_idea_dict key size:%d, value size:%d\n", get_key_num(&del_unit_idea_dict), get_value_num(&del_unit_idea_dict));
    printf("tb_hashmap size:%d\n", ht2.size());

    uint hash_inc_num1 = 0;
    uint node_inc_num1 = 0;
    uint hash_inc_num2 = 0;
    uint node_inc_num2 = 0;

    timer.start();
    del_merge_hash_dict(&unitad_dict, &del_unit_idea_dict, hash_inc_num1, node_inc_num1);
    add_merge_hash_dict(&unitad_dict, &add_unit_idea_dict, hash_inc_num2, node_inc_num2);
    timer.stop();
    printf("HashTemplate & sortedlist merge time:%ldms, hash_inc_num_del:%u, \
node_inc_num_del:%u, hash_inc_num_add:%u, node_inc_num_add:%u\n", timer.m_elapsed(), hash_inc_num1, 
           node_inc_num1, hash_inc_num2, node_inc_num2);
    
    //tb_hashmap & cowbass
    list_count = 0;
    timer.start();

    unitad_dict_tb::iterator it = ht.begin();
    for (; it != ht.end(); ++it)
    {
        unitad_list_cowbass *list_tmp = new unitad_list_cowbass(&mp);
        //FIXME, following line is commented to enable compiling
        //it->second()->spawn(list_tmp);
        ht2.set(it->first(), list_tmp);
        list_count += list_tmp->count();
    }

    ht2.spawn(ht);
    timer.stop();
    printf("tb_hashmap & cowbass merge time:%ldms, list size:%d\n", timer.m_elapsed(), list_count);
}

/**
// Brief: 
 * @begin_version 
 **/
TEST_F(test_perf_suite, lookup)
{
    std::vector<int> ids(UNIT_COUNT);
    
    for (int i = 0; i < UNIT_COUNT; ++i)
    {
        ids.push_back(rand()%UNIT_COUNT);
    }

    int list_size = 0;
    naive_timer_t timer;
    std::vector<int>::iterator p_id = ids.begin();
    list_size = 0;

    timer.start();
    for (; p_id != ids.end(); ++p_id)
    {
        BS_AFS_LIB::cpro_sign_t snode;
        snode.sign1 = *p_id;
        snode.sign2 = BS_AFS_LIB::BD_DEFAULT_SIGN2;

        BS_AFS_LIB::unitad_dict_t::iterator plist = unitad_dict.find(snode);
        if (plist != unitad_dict.end())
        {
            BS_AFS_LIB::unitad_list_t::iterator iter =plist->second->begin();
            for (; iter != plist->second->end(); ++iter)
                ;
            list_size += plist->second->count();
        }
    }
    timer.stop();
    printf("Hashtemplate & Sortedlist lookup:%ldms, %d\n", timer.m_elapsed(), list_size);

    unitad_dict_tb::iterator it;
    list_size = 0;
    timer.start();
    for (p_id = ids.begin(); p_id != ids.end(); ++p_id)
    {
        if ((it = ht.find(*p_id)) != ht.end())
        {
            unitad_list_cowbass::iterator it2 = it->second()->begin();
            list_size += it->second()->count();
            for (; it2 != it->second()->end(); ++it2)
                ;
        }
    }
    timer.stop();
    printf("tb_hashmap & cowbass lookup:%ldms, %d\n", timer.m_elapsed(), list_size);

}

/**
// Brief: 
 **/
class test_tb_hashmap_suite : public ::testing::Test{
protected:
    test_tb_hashmap_suite(){};
    virtual ~test_tb_hashmap_suite(){};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};

