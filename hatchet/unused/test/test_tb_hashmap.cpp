
#include <gtest/gtest.h>
#include <tb_hashmap.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace st;	
using namespace std;
struct clear_t
{
    void operator()( int& t )
    {
        (void)t;
        ++ del_;
    }
    static int del_;
};
int clear_t::del_ = 0;

typedef tb_hashmap_t<int, int> hashmap_t;
typedef tb_hashmap_t<int, int, clear_t> hashmap_t2;

template< typename T >
void get_check( T &ht1, T &ht2 )
{
    ht1.spawn( ht2 );
    //ht1.freeze();
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/**
// Brief: 
 **/
class test_usage_suite : public ::testing::Test{
protected:
    test_usage_suite(){};
    virtual ~test_usage_suite(){};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};


/**
// Brief: 
 * @begin_version 
 **/
TEST_F(test_usage_suite, trivial)
{
    srand (time(0));

    hashmap_t ht;
    hashmap_t ht2;
    hashmap_t::create( ht, ht2 );
   
    /// test set()
    ht.set(2,1);
    ht.set(1,1);
    ASSERT_EQ( ht.size(), 0U );

    get_check( ht, ht2 );
    //ASSERT_EQ( show(ht), show(ht2) );
    ASSERT_EQ( ht2.size(), 2U );

    cout << show (ht2) << endl;
    
    ht2.del(1);
    get_check( ht2, ht );
    //ASSERT_EQ( show(ht), show(ht2) );
    ASSERT_EQ( ht.size(), 1U );
    
    /// test del() + begin();
    ASSERT_EQ( ht.begin()->first(), 2 );
    /// test find()
    ASSERT_TRUE( ht.find(1) == ht.end() );
    ASSERT_TRUE( ht.find(2)->first() == 2 && ht.find(2)->second() == 1 );
    
    /// test duplicated set
    ht.set(1,1);
    ht.set(2,1);
    ht.set(3,1); 
    ht.set(4,1);
    ht.del(2);

    get_check( ht, ht2 );
    //ASSERT_EQ( show(ht), show(ht2) );
    ASSERT_EQ( ht2.size(), 3U );
    cout << show(ht2) <<endl;
    
    /// test read only by spawn
    ASSERT_FALSE( ht.del(4) );
    get_check( ht2, ht );
    ASSERT_EQ( show(ht), show(ht2) );
    
    get_check( ht, ht2 );
    for( int i = 1; i < 10; i += 2 )
    {
        ht2.del(i);
    }
    
    get_check( ht2, ht );
    //ASSERT_EQ( show(ht2), show(ht) );
    cout << show(ht) <<endl;
    
    get_check( ht, ht2 );
    //ASSERT_EQ( show(ht), show(ht2) );

    ht2.set(1, 1);
    ht2.del(1);
    ht2.set(3,4);
    ht2.set(1, 2);
    ht2.del(4);
    ht2.set(1, 3);
    ht2.del(3);
    ht2.del(1);
    ht2.set(1, 7);
    ht2.set(4, 2);
    get_check( ht2, ht );
    cout << show(ht) <<endl;
    ASSERT_TRUE( ht.find(1)->first() == 1 && ht.find(1)->second() == 7 );

    hashmap_t::destroy( ht, ht2 );
}

TEST_F(test_usage_suite, clear_func_test)
{
    clear_t clear;
    hashmap_t2 ht;
    hashmap_t2 ht2;
    hashmap_t2::create( ht, ht2, 10, 0, clear);

    ht.set(1,1);
    ht.set(1,2);
    get_check( ht, ht2 );
    ASSERT_EQ( clear.del_, 0 );
    
    ht2.set(1,3);
    get_check( ht2, ht );
    ASSERT_EQ( clear.del_, 1 );
    
    ht.set(2,1);
    get_check( ht, ht2 );
    ASSERT_EQ( clear.del_, 2 );

    ht2.del(1);
    get_check( ht2, ht );
    ASSERT_EQ( clear.del_, 3 );
    
    ht.set(1,4);
    get_check( ht, ht2 );
    ASSERT_EQ( clear.del_, 4 );
    hashmap_t2::destroy( ht, ht2 );
}

/**
// Brief: ≤‚ ‘¿‡
// Author:      zhuxingchang@baidu.com
// Date:        08/10/2010
 **/
struct foo
{
    int     a;
    int     b;
    void to_string(StringWriter &sb) const
    {
        sb << "(" << a << "," << b << ")";
    }
    explicit foo( int a, int b)
    {
        this->a = a;
        this->b = b;
    }
    foo()
    {
        a=0;
        b=0;
    }       
    bool operator <(const foo &other)const
    {       
        if ( a < other.a )
            return true;
        else if ( a > other.a )
            return false;
        else
            if ( b < other.b )
                return true;
        return false;
    }
    bool operator >(const foo &other)const
    {
        if ( a > other.a )
            return true;
        else if ( a < other.a )
            return false;
        else
            if ( b > other.a )
                return true;
        return false;
    }
    bool operator ==(const foo &other) const
    {
        if ( a==other.a && b==other.b)
            return true;
        else
            return false;
    }
};

/**
 * @brief	test begin func
 * @begin_version	1.0.0.0
 * @author	zhuxingchang@baidu.com
 * @date	08/11/2010
 **/
TEST_F(test_usage_suite, begin_basic_type)
{
    typedef tb_hashmap_t<int,int>	zxc_hashmap_t;
    zxc_hashmap_t	ht1;
    zxc_hashmap_t	ht2;
    zxc_hashmap_t::create(ht1, ht2);

    ht1.set(1,1);
    ht1.set(2,2);
    ht1.spawn(ht2);
    ASSERT_EQ(1, ht2.begin()->first());
    ASSERT_EQ(1, ht2.begin()->second());

    ht2.set(1,3);
    ht2.spawn(ht1);
    ASSERT_EQ(1, ht1.begin()->first());
    ASSERT_EQ(3, ht1.begin()->second());
}

/**
// Brief:       test begin func
 * @begin_version       1.0.0.0
// Author:      zhuxingchang@baidu.com
// Date:        08/11/2010
 **/
TEST_F(test_usage_suite, begin_struct_type)
{
    typedef tb_hashmap_t<int,foo>   zxc_hashmap_t;
    zxc_hashmap_t   ht1;
    zxc_hashmap_t   ht2;
    zxc_hashmap_t::create(ht1, ht2);

    ht1.set(1,foo(1,1));
    ht1.set(2,foo(2,2));
    ht1.spawn(ht2);
    ASSERT_EQ(1, ht2.begin()->first());
    ASSERT_STREQ("(1,1)", show(ht2.begin()->second()).c_str());

    //dup set
    ht2.set(3,foo(3,3));
    ht2.set(1,foo(4,4));
    ht2.spawn(ht1);
    ASSERT_EQ(1,ht1.begin()->first());
    ASSERT_STREQ("(4,4)", show(ht1.begin()->second()).c_str());
}


/**
 * @brief	test empty func
 * @begin_version	1.0.0.0
 * @author	zhuxingchang@baidu.com
 * @date	08/11/2010
 **/
TEST_F(test_usage_suite, empty_and_size)
{
    typedef tb_hashmap_t<int,foo>   zxc_hashmap_t;
    zxc_hashmap_t   ht1;
    zxc_hashmap_t   ht2;
    zxc_hashmap_t::create(ht1, ht2);

    ht1.set(1,foo(1,1));
    ht1.set(2,foo(2,2));
    ASSERT_TRUE(ht1.empty());
    ASSERT_EQ(0u, ht1.size());

    ht1.spawn(ht2);
    ASSERT_FALSE(ht2.empty());
    ASSERT_EQ(2u, ht2.size());

    ht2.del(1);
    ht2.del(2);
    ASSERT_FALSE(ht2.empty());
    ASSERT_EQ(2u, ht2.size());

    ht2.spawn(ht1);
    ASSERT_TRUE(ht1.empty());
    ASSERT_EQ(0u, ht1.size());
}


/**
 * @brief	test set func
 * @begin_version	1.0.0.0
 * @author	zhuxingchang@baidu.com
 * @date	08/11/2010
 **/
TEST_F(test_usage_suite, set)
{

    typedef tb_hashmap_t<string, foo>   zxc_hashmap_t;
    zxc_hashmap_t   ht1;
    zxc_hashmap_t   ht2;
    zxc_hashmap_t::create(ht1, ht2);

    ht1.set(string("a"), foo(1,1));
    ht1.set(string("b"), foo(2,2));
    ht1.spawn(ht2);
    ASSERT_STREQ("a", show(ht2.begin()->first()).c_str());
    ASSERT_STREQ("(1,1)", show(ht2.begin()->second()).c_str());

	
}
/**
// Brief:       test set func
 * @begin_version       1.0.0.0
// Author:      zhuxingchang@baidu.com
// Date:        08/11/2010
 **/
TEST_F(test_usage_suite, regression_set)
{
    typedef tb_hashmap_t<int, foo>   zxc_hashmap_t;
    zxc_hashmap_t   ht1;
    zxc_hashmap_t   ht2;
    zxc_hashmap_t::create(ht1, ht2);

    for (int i=0; i< 100000; i++ )
    {
        ht1.set(i, foo(i,i));
    }
    ASSERT_TRUE(ht1.empty());

    ht1.spawn(ht2);
    ASSERT_EQ(100000u, ht2.size());
}

/**
// Brief:       test set func
 * @begin_version       1.0.0.0
// Author:      zhuxingchang@baidu.com
// Date:        08/11/2010
 **/
TEST_F(test_usage_suite, dup_set)
{
    typedef tb_hashmap_t<int, foo>   zxc_hashmap_t;
    zxc_hashmap_t   ht1;
    zxc_hashmap_t   ht2;
    zxc_hashmap_t::create(ht1, ht2);

    for (int i=0; i< 100000; i++ )
    {
        ht1.set(i, foo(i,i));
    }
    ht1.spawn(ht2);
    ASSERT_EQ(100000u, ht2.size());

	
    for (int i=0; i< 100000; i++ )
    {
        ht2.set(i, foo(i,i));
    }
    ht2.spawn(ht1);
    ASSERT_EQ(100000u, ht1.size());
}

/**
// Brief:       test del func
 * @begin_version       1.0.0.0
// Author:      zhuxingchang@baidu.com
// Date:        08/11/2010
 **/
TEST_F(test_usage_suite, dup_del)
{
    typedef tb_hashmap_t<int, foo>   zxc_hashmap_t;
    zxc_hashmap_t   ht1;
    zxc_hashmap_t   ht2;
    zxc_hashmap_t::create(ht1, ht2);

    for ( int i = 0; i < 100; i++ )
    {
        ht1.set(i,foo(i,i));
    }

    ht1.spawn(ht2);
    for(int i=0; i < 200; i++)
    {
        ht2.del(i);
    }

    ht2.spawn(ht1);
    ASSERT_EQ(0U, ht1.size());
    ASSERT_TRUE(ht1.empty());

}

/**
// Brief:       test del func
 * @begin_version       1.0.0.0
// Author:      zhuxingchang@baidu.com
// Date:        08/11/2010
 **/
TEST_F(test_usage_suite, basic_find)
{
    typedef tb_hashmap_t<int, int> zxc_hashmap_t;
    typedef zxc_hashmap_t::iterator zxc_iterator;
    zxc_hashmap_t	ht1;
    zxc_hashmap_t	ht2;
    zxc_hashmap_t::create(ht1, ht2);

    ht1.set(1,1);
    ht1.set(2,2);
    ht1.set(3,3);
    ht1.spawn(ht2);

    //found
    zxc_iterator iter = ht2.find(1);
    ASSERT_TRUE(iter != ht2.end());
    ASSERT_EQ(1, iter->first());
    ASSERT_EQ(1, iter->second());

    //not found
    iter = ht2.find(4);
    ASSERT_TRUE(iter == ht2.end());

    iter = ht2.find(4294967296);
    ASSERT_TRUE(iter == ht2.end());
}

/**
// Brief:       test del func
 * @begin_version       1.0.0.0
// Author:      zhuxingchang@baidu.com
// Date:        08/11/2010
 **/
TEST_F(test_usage_suite, struct_find)
{
    typedef tb_hashmap_t<int, foo> zxc_hashmap_t;
    typedef zxc_hashmap_t::iterator zxc_iterator;
    zxc_hashmap_t   ht1;
    zxc_hashmap_t   ht2;
    zxc_hashmap_t::create(ht1, ht2);

    ht1.set(1,foo(1,1));
    ht1.set(2,foo(2,2));
    ht1.set(3,foo(3,3));
    ht1.spawn(ht2);

    //found
    zxc_iterator iter = ht2.find(1);
    ASSERT_TRUE(iter != ht2.end());
    ASSERT_EQ(1, iter->first());
    ASSERT_STREQ("(1,1)", show(iter->second()).c_str());

    //not found
    iter = ht2.find(4);
    ASSERT_TRUE(iter == ht2.end());

    iter = ht2.find(4294967296);
    ASSERT_TRUE(iter == ht2.end());
}

/**
// Brief:       test basic sort func
 * @begin_version       1.0.0.0
// Author:      zhuxingchang@baidu.com
// Date:        08/11/2010
 **/
TEST_F(test_usage_suite, basic_sort)
{
    typedef tb_hashmap_t<int, int> zxc_hashmap_t;
    typedef zxc_hashmap_t::iterator zxc_iterator;
    zxc_hashmap_t   ht1;
    zxc_hashmap_t   ht2;
    zxc_hashmap_t::create(ht1, ht2);

    ht1.set(1,1);
    ht1.set(3,1);
    ht1.set(10,10);
    ht1.set(5,1);
    ht1.spawn(ht2);
    ASSERT_STREQ("[(1,1),(3,1),(5,1),(10,10)]", show(ht2).c_str());	
}
/**
// Brief:       test sort func
 * @begin_version       1.0.0.0
// Author:      zhuxingchang@baidu.com
// Date:        08/11/2010
 **/
TEST_F(test_usage_suite, stuct_sort)
{
    /*
      typedef tb_hashmap_t<foo, int> zxc_hashmap_t;
      typedef zxc_hashmap_t::iterator zxc_iterator;
      zxc_hashmap_t   ht1;
      zxc_hashmap_t   ht2;
      zxc_hashmap_t::create(ht1, ht2);

      //basic sort
      ht1.set(foo(1,1), 1);
      ht1.set(foo(1,2), 2);
      ht1.set(foo(2,2), 3);
      ht1.spawn(ht2);
      ASSERT_EQ(3u, ht2.size());
      ASSERT_STREQ("(1,1)", show(ht2.begin()->second()).c_str());

      //user-defined sort
      zxc_iterator	iter = ht2.find(foo(2,2));
      ASSERT_EQ(3, iter->second());
      ASSERT_STREQ("(2,2)", show(iter->first()).c_str());
    */
}

/**
// Brief:       test basic sort func
 * @begin_version       1.0.0.0
// Author:      zhuxingchang@baidu.com
// Date:        08/11/2010
 **/
TEST_F(test_usage_suite, clear)
{
    typedef tb_hashmap_t<int, int> zxc_hashmap_t;
    typedef zxc_hashmap_t::iterator zxc_iterator;
    zxc_hashmap_t   ht1;
    zxc_hashmap_t   ht2;
    zxc_hashmap_t::create(ht1, ht2);

    ht1.set(1,1);
    ht1.set(2,2);
    ht1.spawn(ht2);
    ht2.clear();
    ASSERT_EQ(0U, ht2.size());
	
}

/**
// Brief:       test basic sort func
 * @begin_version       1.0.0.0
// Author:      zhuxingchang@baidu.com
// Date:        08/11/2010
 **/
TEST_F(test_usage_suite,spawn)
{
    typedef tb_hashmap_t<int, int> zxc_hashmap_t;
    typedef zxc_hashmap_t::iterator zxc_iterator;
    zxc_hashmap_t   ht1;
    zxc_hashmap_t   ht2;
    zxc_hashmap_t::create(ht1, ht2);

    ht1.set(1,1);
    ht1.set(2,2);
    ht1.spawn(ht2);
    ht2.set(2,4);
    ht2.set(1,2);
    ht2.spawn(ht1);
    ASSERT_EQ(2u, ht1.size());
    ASSERT_STREQ("[(1,2),(2,4)]", show(ht1).c_str());

    ht1.del(1);
    ht1.spawn(ht2);
    ASSERT_STREQ("[(2,4)]", show(ht2).c_str());
}

struct self_spawn_t
{
    int operator() (const int &x){return (int)2*x;}
    int operator() (const int &x)const{return (int)2*x;}
};
/**
// Brief:       test basic sort func
 * @begin_version       1.0.0.0
// Author:      zhuxingchang@baidu.com
// Date:        08/11/2010
 **/
TEST_F(test_usage_suite, self_spawn)
{
    typedef tb_hashmap_t<int, int,identity<int>, hash<int>, is_equal<int>, compare<int>, self_spawn_t> zxc_hashmap_t;
    typedef zxc_hashmap_t::iterator	zxc_iterator_t;
    
    zxc_hashmap_t	ht1;
    zxc_hashmap_t	ht2;
    zxc_hashmap_t::create(ht1,ht2,10,0,identity<int>(), self_spawn_t());
    
    ht1.set(1,1);
    ht1.set(2,2);
    ht1.spawn(ht2);
    //ASSERT_STREQ("[(1,2),(2,4)]",show(ht2).c_str());	
    zxc_iterator_t iter = ht2.begin();
    ASSERT_TRUE(iter->first() == 1 && iter->second() == 2);
    ++iter;
    ASSERT_TRUE(iter->first() == 2 && iter->second() == 4);
	
}
