/***************************************************************************
 * 
 * Copyright (c) 2011 Baidu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file test_stack.cpp
 * @author work(qianyuhao@baidu.com)
 * @date 2011/06/07 16:33:46
 * @version $Revision$
 * @brief 
 *  
 **/

#include <gtest/gtest.h>
#include "table_stack.hpp"
#include "basic_tuple.hpp"
//#include "group_view.hpp"
using namespace st;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

DEFINE_COLUMN(UNIT_ID, unsigned int);
DEFINE_COLUMN(PRICE, int);
DEFINE_COLUMN(ADQ, int);
DEFINE_COLUMN(KEYWORD_ID, int);

typedef ST_TABLE(UNIT_ID, PRICE, ST_UNIQUE_KEY(UNIT_ID)) tbl_up;
typedef ST_TABLE(UNIT_ID, ADQ, ST_UNIQUE_KEY(UNIT_ID)) tbl_uq;
typedef ST_TABLE(UNIT_ID, KEYWORD_ID, ST_UNIQUE_KEY(UNIT_ID,
    ST_REVERSED_CLUSTER_KEY(KEYWORD_ID, MaxFanout<16>))) tbl_in;
//typedef GroupView<tbl_in> UnitKeywordGView;

typedef ST_TABLE(UNIT_ID, ADQ, ST_UNIQUE_KEY(UNIT_ID)) tbl_uq;
typedef ST_TABLE(UNIT_ID, ADQ, ST_UNIQUE_KEY(UNIT_ID)) tbl_uq;
typedef ST_TABLE(UNIT_ID, PRICE, ADQ, ST_UNIQUE_KEY(UNIT_ID)) tbl_join;
typedef ST_CONNECTOR(tbl_join, 
        ST_PICK(TBL1<UNIT_ID>, TBL1<PRICE>, TBL2<ADQ>),
        ST_FROM(tbl_up, tbl_uq),
        ST_WHERE(eq(TBL1<UNIT_ID>, TBL2<UNIT_ID>))) tbl_con;

/*
typedef ST_CONNECTOR(tbl_join, 
        ST_PICK(TBL1<UNIT_ID>, TBL1<PRICE>),
        ST_FROM(tbl_up, Exclude<UnitKeywordGView>)) tbl_con2;
*/
TEST(test_table_stack, basic)
{
    TableStack<tbl_up> t1;
    t1.add_table(0);
    t1.add_table(2);
    ASSERT_EQ(2U, t1.count());
}

TEST(test_connector_stack, basic_all_single)
{
    const tbl_up t1;
    const tbl_uq t2;
    TableStack<tbl_join> t3;
    ConnectorStack<tbl_con> con(&t3, &t1, &t2);
    ASSERT_EQ(1u, t3.count());
}

TEST(test_connector_stack, basic_all_plural_const)
{
    TableStack<tbl_up> t1;
    t1.add_table(0);
    t1.add_table(2);
    TableStack<tbl_uq> t2;
    t2.add_table(0);
    t2.add_table(2);
    TableStack<tbl_join> t3;
    const TableStack<tbl_up> * pt1 = &t1;
    const TableStack<tbl_uq> * pt2 = &t2;
    ConnectorStack<tbl_con> con(&t3, pt1, pt2);
    ASSERT_EQ(4u, t3.count());
    ASSERT_TRUE(t3.get_table(t1.get_table_desc(0)->sid ^ t2.get_table_desc(2)->sid));
}

TEST(test_connector_stack, basic_all_plural)
{
    TableStack<tbl_up> t1;
    t1.add_table(0);
    t1.add_table(2);
    TableStack<tbl_uq> t2;
    t2.add_table(0);
    t2.add_table(2);
    TableStack<tbl_join> t3;
    ConnectorStack<tbl_con> con(&t3, &t1, &t2);
    ASSERT_EQ(4U, t3.count());
    ASSERT_TRUE(t3.get_table(t1.get_table_desc(0)->sid ^ t2.get_table_desc(2)->sid));
}

TEST(test_connector_stack, basic_mixed)
{
    TableStack<tbl_up> t1;
    t1.add_table(0);
    t1.add_table(2);
    const tbl_uq t2;
    TableStack<tbl_join> t3;
    ASSERT_FALSE(t3.get_table(t1.get_table_desc(0)->sid));
    ConnectorStack<tbl_con> con(&t3, &t1, &t2);
    ASSERT_EQ(2U, t3.count());
    ASSERT_TRUE(t3.get_table(t1.get_table_desc(0)->sid));
}

TEST(test_connector_stack, basic_mixed_const)
{
    TableStack<tbl_up> t1;
    t1.add_table(0);
    t1.add_table(2);
    const tbl_uq t2;
    TableStack<tbl_join> t3;
    ASSERT_FALSE(t3.get_table(t1.get_table_desc(0)->sid));
    const TableStack<tbl_up> * pt1 = &t1;
    ConnectorStack<tbl_con> con(&t3, pt1, &t2);
    ASSERT_EQ(2u, t3.count());
    ASSERT_TRUE(t3.get_table(t1.get_table_desc(0)->sid));
}

TEST(test_connector_stack, basic_mixed1)
{
    const tbl_up t1;
    TableStack<tbl_uq> t2;
    t2.add_table(0);
    t2.add_table(2);
    TableStack<tbl_join> t3;
    ASSERT_FALSE(t3.get_table(t2.get_table_desc(0)->sid));
    const TableStack<tbl_uq> * pt2 = &t2;
    ConnectorStack<tbl_con> con(&t3, &t1, pt2);
    ASSERT_EQ(2u, t3.count());
    ASSERT_TRUE(t3.get_table(t2.get_table_desc(0)->sid));
}


TEST(test_connector_stack, basic_t2t)
{
    TableStack<tbl_up> t1;
    t1.add_table(0);
    t1.add_table(2);
    TableStack<tbl_uq> t2;
    t2.add_table(0);
    t2.add_table(2);
    t2.make_t2t(t1);
    TableStack<tbl_join> t3;
    ConnectorStack<tbl_con> con(&t3, &t1, &t2);
    /*t1.print();
    t2.print();
    t3.print();*/
    ASSERT_EQ(2U, t3.count());
    ASSERT_TRUE(t3.get_table(t1.get_table_desc(0)->sid));
}


TEST(test_connector_stack, copy_construct)
{
    TableStack<tbl_up> t1;
    t1.add_table(0);
    t1.add_table(2);
    TableStack<tbl_up> t2(t1);
    ASSERT_EQ(2U, t2.count());
    ASSERT_TRUE(t2.get_table_desc(2));
}


TEST(test_connector_stack, assign)
{
    TableStack<tbl_up> t1;
    t1.add_table(0);
    t1.add_table(2);
    TableStack<tbl_up> t2;
    t2 = t1;
    ASSERT_EQ(2U, t2.count());
    ASSERT_TRUE(t2.get_table_desc(2));
}










/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
