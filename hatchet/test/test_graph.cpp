// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: yanlin@baidu.com
// Date: 2010-08-06 10:18

#include <gtest/gtest.h>
#include <sstream>
#include "graph.hpp"

using namespace st;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class test_graph_suite : public ::testing::Test{
protected:
    test_graph_suite(){};
    virtual ~test_graph_suite(){};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};

TEST_F(test_graph_suite, constructor)
{
    char EXPECTED[] =
        "1:->2->3->4\n"
        "2:->4\n"
        "3:->4\n"
        "4:->1\n";
    graph_t<int> g;
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    g.add_edge(1, 4);
    g.add_edge(2, 4);
    g.add_edge(3, 4);
    g.add_edge(4, 1);
    
    ASSERT_STREQ(EXPECTED, show(g).c_str());
}

TEST_F(test_graph_suite, adjacency_list)
{
    int N = 5;
    typedef graph_t<int> Graph;
    graph_t<int>g;

    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            g.add_edge(i, j);        
        }
    }

    for (int i = 0; i < N; ++i)
    {
        Graph::adjacency_iterator_pair iters = g.get_adjacency_list(i);
        Graph::adjacency_iterator it = iters.first;
        Graph::adjacency_iterator end = iters.second;
        
        for (int j = 0; it != end; ++it, ++j)
        {
            ASSERT_EQ(j, *it);
        }        
    }
}

template <typename T>
struct print_visitor_t : dfs_visitor_t
{
    void start_vertex(const T&t) 
    {
        ss << "start:" << t << "->";
    }
    void back_edge(const T&t)
    {
        ss << "back:" << t << "->";
    }
    void finish_vertex(const T&t)
    {
        ss << "finish:" << t << "->";
    }
    void forward_or_cross_edge(const T&t)
    {
        ss << "forward:" << t << "->";
    }
    std::stringstream ss;
};

TEST_F(test_graph_suite, dfs)
{
    const char EXPECTED_RESULT[] = "start:1->start:2->start:4->finish:4->start:3->forward:4->finish:3->finish:2->forward:3->forward:4->finish:1->";

    graph_t<int> g;
    print_visitor_t<int> v;
    g.add_edge(1, 2);
    g.add_edge(1, 3);
    g.add_edge(1, 4);
    g.add_edge(2, 4);
    g.add_edge(3, 4);
    g.add_edge(2, 3);

    depth_first_search(g, v);
    
    ASSERT_STREQ(EXPECTED_RESULT, v.ss.str().c_str());
}

