
#include <gtest/gtest.h>
#include <allocators.hpp>
#include <vector>

using namespace st;

int main(int argc, char **argv)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
/**
// Brief: 
**/
class test_allocators_suite : public ::testing::Test{
    protected:
        test_allocators_suite(){};
        virtual ~test_allocators_suite(){};
        virtual void SetUp() {
        };
        virtual void TearDown() {
        };
};


struct foo
{
    int x;
    int y;
    std::vector<int> a_val;
};

/**
// Brief: 
 * @begin_version 
**/
TEST_F(test_allocators_suite, case_name1)
{
    //TODO
    typedef allocator_t<foo> alloc1_t;
    alloc1_t alloc1;
    for (int j=0; j < 100; ++j)
    {
        alloc1_t::Pointer p1 = alloc1.allocate ();
        ASSERT_EQ (p1->a_val.size(), 0ul);
        size_t i=0;
        for (i=0; i < 100; ++i)
        {
            p1->a_val.push_back (i);
        }
        ASSERT_EQ (p1->a_val.size(), i);

        alloc1.deallocate(p1);
        alloc1.recycle_delayed();
    }

    typedef allocator_t<foo,MP0_ID> alloc2_t;
    alloc2_t alloc2;
    for (int j=0; j < 100; ++j)
    {
        alloc2_t::Pointer v_p1 = alloc2.allocate();
        foo* p1 = alloc2.value_ptr(v_p1);
        ASSERT_EQ (p1->a_val.size(), 0ul);
        size_t i=0;
        for (i=0; i < 100; ++i)
        {
            p1->a_val.push_back (i);
        }
        ASSERT_EQ (p1->a_val.size(), i);

        alloc2.deallocate(v_p1);
        alloc2.recycle_delayed();
    }

}
 
