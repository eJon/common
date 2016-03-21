// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-08-02 07:24

#include <gtest/gtest.h>
#include <string_writer.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <st_timer.h>

using namespace st;
	
int main(int argc, char **argv)
{
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

class test_usage_suite : public ::testing::Test{
    protected:
        test_usage_suite(){};
        virtual ~test_usage_suite(){};
        virtual void SetUp() {
        };
        virtual void TearDown() {
        };
};

struct ref_t
{
    std::string s;
    StringWriter sw;

    bool append (const char* str)
    {
        s += str;
        //sw.append_format ("%s", str);
        sw << str;
        if (0 == strcmp (s.c_str(), sw.c_str())) {
            return true;
        } else {
            printf ("s=%s, sw=%s\n", s.c_str(), sw.c_str());
            return false;
        }
    }

    bool reset (const char* str)
    {
        s = str;
        sw.format ("%s", str);
        return (0 == strcmp (s.c_str(), sw.c_str()));
    }
    
    bool clear ()
    {
        s.clear();
        sw.clear ();
        return (0 == strcmp (s.c_str(), sw.c_str()));
    }
};

TEST_F(test_usage_suite, empty_string_core)
{
    StringWriter sw;
    sw << "";
    printf ("SEEING THIS LINE MEANS GOOD %s\n", sw.c_str());
}

TEST_F(test_usage_suite, regression)
{
    srand (time(0));

    ref_t ref;
    char buf[10240];
    int len=0;
    

    ASSERT_TRUE(ref.append("987654321"));
    for (int i=0;i<10000; ++i) {
        len = 0;
        for (int j=20; j>0; --j) {
            len+=snprintf (buf+len, 10240-len, "[%d]", rand());
        }
        unsigned int r = rand () % 20;
        if (r == 0) {
            ASSERT_TRUE(ref.reset(buf));
        } else if (r == 1) {
            ASSERT_TRUE (ref.clear());
        } else {
            ASSERT_TRUE(ref.append(buf));
        }
        //printf ("sw.info: %s\n", ref.sw.info().c_str());
    }

    ASSERT_TRUE(ref.clear());
    for (int i=0;i<100; ++i) {
        snprintf (buf, 10240, "%d", rand()%10);
        ASSERT_TRUE(ref.append(buf));
        //printf ("sw.info: %s, %s\n", ref.sw.info().c_str(), ref.sw.c_str());
    }
    
    ASSERT_TRUE(ref.reset ("body=20:[(0,1),(1,1),(2,5),"));
    for (int i=0;i<10; ++i) {
        ASSERT_TRUE(ref.append ("8"));
        //        ASSERT_TRUE(ref.append (")"));
    }
    
    
}

TEST_F(test_usage_suite, performance)
{
    NaiveTimer t;

    const int times = 100000;
    int* a_rand = new int[times];
    for (int i=0; i<times; ++i) {
        a_rand[i] = rand();
    }
    
    t.start();
    StringWriter sw1;
    t.stop();
    cout << "unreserved setup time=" << t.u_elapsed() << "us" << endl; 

    t.start();
    for (int i=0; i < times; ++i ) {
        sw1 << a_rand[i];
    }
    t.stop();
    cout << "unreserved time=" << t.u_elapsed() << "us"
         << ", per_char=" << t.u_elapsed()*1000/sw1.length() << "ns"
         << endl;

    StringWriter sw1_prime = sw1;
    ASSERT_EQ (0, strcmp (sw1.c_str(), sw1_prime.c_str()));
    ASSERT_EQ (sw1.length(), sw1_prime.length());

    t.start();
    StringWriter sw2;
    t.stop();
    cout << "reserved setup time=" << t.u_elapsed() << "us" << endl; 
    t.start();
    for (int i=0; i<times; ++i) {
        sw2 << a_rand[i];
    }
    t.stop();
    cout << "reserved time=" << t.u_elapsed() << "us"
         << ", per_char=" << t.u_elapsed()*1000/sw2.length() << "ns"
         << endl; 

    sw1_prime = sw2;
    ASSERT_EQ (0, strcmp (sw2.c_str(), sw1_prime.c_str()));
    ASSERT_EQ (sw2.length(), sw1_prime.length());

    t.start();
    std::ostringstream oss;
    t.stop();
    cout << "oss setup time=" << t.u_elapsed() << "us" << endl; 
    t.start();
    for (int i=0; i<times; ++i) {
        oss << a_rand[i];
    }
    t.stop();
    cout << "oss time=" << t.u_elapsed() << "us"
         << ", per_char=" << t.u_elapsed()*1000/oss.tellp() << "ns"
         << endl; 

    sw1_prime = sw2;
    ASSERT_EQ (0, strcmp (sw2.c_str(), sw1_prime.c_str()));
    ASSERT_EQ (sw2.length(), sw1_prime.length());

    ASSERT_EQ (sw1_prime.length(), oss.tellp());
    
    
    const int sz_data = times*10+1;
    char* p_data = new char[sz_data];
    ASSERT_TRUE (p_data != NULL);
    int len=0;
    t.start();
    for (int i=0; i<times; ++i) {
        len += snprintf (p_data, sz_data-len, "%d", a_rand[i]);
    }
    t.stop();
    cout << "directly_format_time=" << t.u_elapsed() << "us"
         << ", per_char=" << t.u_elapsed()*1000/len << "ns"
         << endl; 
    delete [] p_data;

    
    ASSERT_EQ (sw1.length(), len);
    ASSERT_EQ (sw2.length(), len);

    delete [] a_rand;
}
