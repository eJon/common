// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-08-18 11:24

#include <gtest/gtest.h>
#include <string_reader.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <common.h>

using namespace st;

// Copied from app/ecom/nova/public/util/utils.cpp as performance reference
int split_string(char* string1, char** fields, int nfields, char const* sep)
{
    if (NULL == sep) {
        return -1;
    } 
	
    int len = strlen(sep);
    if (len <= 0) {
        return -1;
    }

    int num = 0;
    fields[0] = string1;
    char* start = string1;
    num = 1;
    for (int i = 0; i < nfields-1; i++) {
        char* pos = strstr(start, sep);
        if (NULL == pos) {
            break;
        }
        
        // for (int j = 0; j < len; j++) {
        //     pos[j] = 0;
        // }
        start = pos + len;
        fields[i+1] = start;
        num ++;
    }
	
    return num;
}

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

TEST_F(test_usage_suite, bug_side_effect)
{
    std::vector<int> xs;
    ASSERT_TRUE(
        (StringReader("10;") >> ((read_push(xs) >> ',') | (read_push(xs) >> ';'))).good());
    ASSERT_EQ(1ul, xs.size());
}

TEST_F(test_usage_suite, parse_bool)
{
    bool dummy = false;
    
    ASSERT_TRUE((StringReader("true1") >> &dummy).good() && dummy);
    ASSERT_TRUE((StringReader("TRUE3") >> &dummy).good() && dummy);
    ASSERT_TRUE((StringReader("false") >> &dummy).good() && !dummy);
    ASSERT_TRUE((StringReader("FALSE") >> &dummy).good() && !dummy);
    ASSERT_FALSE((StringReader("False") >> &dummy).good());
    ASSERT_FALSE((StringReader("F") >> &dummy).good());
    ASSERT_FALSE((StringReader("T") >> &dummy).good());
}

TEST_F(test_usage_suite, check_string)
{
    
    srand(time(0));
    NaiveTimer tm;

    const int N = 1000;
    const char* patterns[] = { "afkd*#UyffdOS", "fjd%k7^fLlajA", "2r4@2_+j43k12j" };
    size_t npatterns = sizeof(patterns) / sizeof(const char*);
    char* buf = new char[N * 50];  // enough for N (decimal) int
    char* head = buf;
    size_t total_len = 0;

    for (int i = 0; i < N; ++ i) {
        for (size_t j = 0; j < npatterns; ++j) {
            const size_t len = sprintf(head, "%s ", patterns[j]);
            head += len;
            total_len += len;
        }
    }
    *head = '\0';
    
    tm.start();
    StringReader s(buf);
    s >> many1(once(patterns[0]) >> ' ' >> patterns[1] >> ' ' >> patterns[2] >> ' ')
      >> sr_ends;
    tm.stop();
    cout << "Elapse: " << tm.n_elapsed()/(double)N << "ns/check" << endl
         << "Tp: " << total_len * 1000.0 / tm.n_elapsed() << "MB/s" << endl;

    // Verify
    ASSERT_TRUE(s.good());


    delete[] buf;
}

TEST_F(test_usage_suite, parse_int)
{
    srand(time(0));
    NaiveTimer tm;

    const int N = 10000;
    char* buf = new char[N * 12];  // enough for N (decimal) ints
    char* head = buf;
    std::vector<int> nums;
    std::vector<const char*> strs;
    size_t total_len = 0;

    nums.reserve(N);
    strs.reserve(N);
    for (int i = 0; i < N; ++ i) {
        nums.push_back(rand() * 977);
        strs.push_back(head);
        const size_t len = sprintf(head, "%d", nums.back());
        total_len += len + 1;
        head += len + 1;
    }

    tm.start();
    for (int i = 0; i < N; ++ i) {
        int dummy;
        StringReader(strs[i]) >> &dummy;
    }
    tm.stop();
    cout << "Elapse: " << tm.n_elapsed()/(double)N << "ns/int" << endl
         << "Tp: " << total_len * 1000.0 / tm.n_elapsed() << "MB/s" << endl;

    // Verify
    for (int i = 0; i < N; ++ i) {
        int dummy;
        StringReader(strs[i]) >> &dummy;
        ASSERT_EQ(nums[i], dummy);
    }
}

TEST_F(test_usage_suite, parse_double)
{
    srand(time(0));
    NaiveTimer tm;

    const int N = 10000;
    char* buf = new char[N * 40];  // enough for N doubles
    char* head = buf;
    std::vector<double> nums;
    std::vector<const char*> strs;
    size_t total_len = 0;
    
    nums.reserve(N);
    strs.reserve(N);
    for (int i = 0; i < N; ++ i) {
        nums.push_back(rand() * 977.0);
        strs.push_back(head);
        const size_t len = sprintf(head, "%f", nums.back());
        total_len += len;
        head += len + 1;
    }

    tm.start();
    for (int i = 0; i < N; ++ i) {
        double dummy;
        StringReader(strs[i]) >> &dummy;
    }
    tm.stop();
    cout << "Elapse: " << tm.n_elapsed()/(double)N << "ns/double" << endl
         << "Tp: " << total_len * 1000.0 / tm.n_elapsed() << "MB/s" << endl;

    // Verify
    for (int i = 0; i < N; ++ i) {
        double dummy;
        StringReader(strs[i]) >> &dummy;
        ASSERT_EQ(nums[i], dummy);
    }
}

TEST_F(test_usage_suite, read_until)
{
    srand(time(0));
    NaiveTimer tm;

    const int N = 10000;
    char* buf = new char[N * 40];  // enough for N doubles
    char* ref = new char[N * 40];  // enough for N doubles

    ASSERT_TRUE((StringReader(",;,;") >> skip0(";,") >> sr_ends).good());
    
    char* head = buf;
    char* ref_head = ref;
    size_t total_len = 0;
    for (int i = 0; i < N; ++ i) {
        const size_t len = sprintf(ref_head, "%d", rand() * 977);
        sprintf(head, "%s", ref_head);
        total_len += len + 1;
        ref_head += len + 1;
        head += len;
        for (int j = rand() % 10; j >= 0; --j) {
            *head++ = ';';//((rand()%2) ? ';' : ',');
        }
    }
    *head = '\0';
    //cout << buf << endl;
    //cout << ref << endl;

    char* buf2 = new char[N * 40];
    HAVE_MANIPULATOR(
        read_some, many1(read_until1(';', aux::CopyCharsMutable(buf2)) >> many0(';')));
    tm.start();
    StringReader s(buf);
    s >> read_some >> sr_ends;
    tm.stop();
    cout << "Elapse: " << tm.n_elapsed()/(double)N << "ns/string" << endl
         << "Tp: " << strlen(buf) * 1000.0 / tm.n_elapsed() << "MB/s" << endl;
    
    // Verify
    ASSERT_TRUE(s.good());
    ASSERT_EQ(0, memcmp(ref, buf2, total_len));
}

struct NamedValue {
    char name[32];
    int value;
};

TEST_F(test_usage_suite, read_assignments)
{
    srand(time(0));
    NaiveTimer tm;

    const int N = 10000;
    char* buf = new char[N * 50];  // enough for N doubles
    char* head = buf;
    std::vector<NamedValue> ref;

    for (int i = 0; i < N; ++ i) {
        NamedValue tmp;
        char* h2 = tmp.name;
        for (int j = rand() % 10; j >= 0; --j) {
            h2 += sprintf(h2, "%c", 'a' + (rand() % 25));
        }
        tmp.value = rand() * 977;
        ref.push_back(tmp);
        
        head += sprintf(head, "%s(%d)", tmp.name, tmp.value);
        for (int j = rand() % 5; j >= 0; --j) {
            *head++ = ' ';
        }
    }
    *head = '\0';

    //cout << "Buf: " << buf << endl;

    NamedValue nv;
    std::vector<NamedValue> nvs;
    tm.start();
    HAVE_MANIPULATOR(read_named_value,
                     read_name1(nv.name) >> '(' >> &nv.value >> ')' >> sr_push_back(nv, nvs));
    StringReader s(buf);
    s >> read_named_value >> many0(many1(' ') >> read_named_value);
    tm.stop();
    cout << "Elapse: " << tm.n_elapsed()/(double)N << "ns/assignment" << endl
         << "Tp: " << strlen(buf) * 1000.0 / tm.n_elapsed() << "MB/s" << endl;
    
    // Verify
    ASSERT_TRUE(s.good());
    ASSERT_EQ(ref.size(), nvs.size());
    for (size_t i = 0; i < ref.size(); ++i) {
        ASSERT_EQ(0, strcmp(ref[i].name, nvs[i].name));
        ASSERT_EQ(ref[i].value, nvs[i].value);
    }
}


TEST_F(test_usage_suite, parse_ints_separated_by_commas)
{
    srand(0);
    NaiveTimer tm;
    const int N = 1000;
    ostringstream oss;
    oss << N << ':';
    for (int i = 0; i < N; ++i) {
        if (i != 0) {
            oss << ",";
        }
        oss << rand();
    }

    std::string ref2 = oss.str();
    char* ref = strdup(ref2.c_str());
    //cout << "Ref: " << ref << endl;

    std::vector<int> xs;
    int n = 0;    
    HAVE_MANIPULATOR(
        m1,
        once(&n) >> maybe(once(':') >> read_push(xs) >> many0(once(',') >> read_push(xs))));

    StringReader s1(ref);
    xs.clear();
    s1 >> m1 >> sr_ends;
    cout << s1 << ", n=" << n << endl;
    ASSERT_TRUE(s1.good());
    //ASSERT_EQ(show_container(ref), show_container(xs));
    ASSERT_EQ (n, (int)xs.size());

    // Performance
    tm.start();
    const int REP = 100;
    for (int i = 0; i < REP; ++ i) {
        StringReader s2(ref);
        xs.clear();
        s2 >> once(&n) >> maybe(once(':') >> read_push(xs) >> many0(once(',') >> read_push(xs)));
    }
    tm.stop();
    cout << "Elapse: " << tm.n_elapsed()/(double)REP << "ns/line" << endl
         << "Tp: " << strlen(ref) * REP * 1000.0 / tm.n_elapsed() << "MB/s" << endl;

    // Compare with split_string
    char* fields[N] = { NULL };
    for ( ; *ref && *ref != ':'; ++ ref);  // find comma
    ASSERT_EQ(':', *ref);
    ++ref;

    tm.start();
    for (int j = 0; j < REP; ++j) {
        split_string(ref, fields, N, ",");
        xs.clear();
        for (int i = 0; i < N; ++i) {
            xs.push_back(strtol(fields[i], NULL, 10));
        }
    }
    tm.stop();
    cout << "Elapse: " << tm.n_elapsed()/(double)REP
         << "ns/line (split_string+strtol)" << endl
         << "Tp: " << strlen(ref)*REP*1000.0 / tm.n_elapsed()
         << "MB/s (split_string+strtol)" << endl;
}


struct Term {
    u_int sign1;
    u_int sign2;
    u_int weight;

    Term() {}

    Term(u_int _sign1, u_int _sign2, u_int _weight)
        : sign1(_sign1), sign2(_sign2), weight(_weight) {}

    bool operator== (const Term& rhs) const
    {
        return sign1 == rhs.sign1 && sign2 == rhs.sign2 && weight == rhs.weight;
    }

friend ostream& operator<<(ostream& os, const Term& t)
    {
        return os << t.sign1 << ',' << t.sign2 << ',' << t.weight;
    }
};


struct Plsa {
    u_int topic;
    u_int weight;
    
    Plsa() {}

    Plsa(u_int _topic, u_int _weight)
        : topic(_topic), weight(_weight) {}

    bool operator== (const Plsa& rhs) const
    {
        return topic == rhs.topic && weight == rhs.weight;
    }

friend ostream& operator<<(ostream& os, const Plsa& t)
    {
        return os << t.topic << ',' << t.weight;
    }
};

struct ParseFeatureData {
    size_t event_id;
    int unit_id;
    size_t nterms;
    std::vector<Term> terms;
    size_t nplsas;
    std::vector<Plsa> plsas;

    void reset()
    {
        terms.clear();
        plsas.clear();
    }
};

TEST_F(test_usage_suite, parse_feature)
{
    srand(time(0));
    NaiveTimer tm;
    ostringstream oss;
    ParseFeatureData ref;

    // Prepare reference
    ref.event_id = ((size_t)rand() << 32) | rand();
    ref.unit_id = rand() * 977;
    ref.nterms = (size_t)rand() % 100;
    if (ref.nterms < 90) {
        // 90% percent to be 0, differentiating maybe and either(sr_ends ...
        ref.nterms = 0;  
    }
    ref.nplsas = (size_t)rand() % 50 + 100;

    oss << ref.event_id << "\t" << ref.unit_id;
    oss << "\t" << ref.nterms;
    if (ref.nterms) {
        oss << ':';
        for (size_t i = 0; i < ref.nterms; ++i) {
            if (i != 0) {
                oss << ';';
            }
            ref.terms.push_back(Term(rand(), rand(), rand()));
            oss << ref.terms.back();
        }
    }
    oss << "\t" << ref.nplsas;
    if (ref.nplsas) {
        oss << ':';
        for (size_t i = 0; i < ref.nplsas; ++i) {
            if (i != 0) {
                oss << ';';
            }
            ref.plsas.push_back(Plsa(rand(), rand()));
            oss << ref.plsas.back();
        }
    }
    oss << endl;

    std::string ref2 = oss.str();
    const char* refstr = ref2.c_str();
    //cout << "Ref: " << refstr << endl;

    ParseFeatureData cur;
    Term term;
    Plsa plsa;
    
    HAVE_MANIPULATOR(
        read_term,
        once(&term.sign1) >> ',' >> &term.sign2 >> ',' >> &term.weight >> sr_push_back(term, cur.terms));

    HAVE_MANIPULATOR(
        read_plsa,
        once(&plsa.topic) >> ',' >> &plsa.weight >> sr_push_back(plsa, cur.plsas));
    
    HAVE_MANIPULATOR(
        feature_parser,
        once(&cur.event_id) >> '\t' >> &cur.unit_id >> '\t' >>
        once(&cur.nterms) >> maybe(once(':') >> read_term >> many0(once(';') >> read_term)) >> '\t' >> once(&cur.nplsas) >> maybe(once(':') >> read_plsa >> many0(once(';') >> read_plsa)) >> sr_endl);

    cur.reset();
    StringReader s1(refstr);
    s1 >> feature_parser;
    cout << s1 << endl;
    ASSERT_TRUE(s1.good());
    ASSERT_EQ (cur.nterms, cur.terms.size());
    ASSERT_EQ (cur.nterms, ref.nterms);
    for (size_t i = 0; i < cur.nterms; ++i) {
        ASSERT_EQ(ref.terms[i], cur.terms[i]);
    }
    ASSERT_EQ (cur.nplsas, cur.plsas.size());
    ASSERT_EQ (cur.nplsas, ref.nplsas);
    for (size_t i = 0; i < cur.nplsas; ++i) {
        ASSERT_EQ(ref.plsas[i], cur.plsas[i]);
    }

    // Performance
    const int REP = 1000;
    tm.start();
    for (int i = 0; i < REP; ++ i) {
        cur.reset();
        StringReader(refstr) >> feature_parser;
    }
    tm.stop();
    cout << "Elapse: " << tm.n_elapsed()/(double)REP << "ns/line" << endl
         << "Tp: " << strlen(refstr)*REP*1000.0/(double)tm.n_elapsed() << "MB/s" << endl;
}


TEST_F(test_usage_suite, parse_complex_format_of_ints)
{
    NaiveTimer tm;
    size_t n = 0;
    std::vector<int> ref;
    for (int i = 0; i < 100; ++i) {
        ref.push_back(rand());
    }
    StringWriter sb;
    shows_range(sb, ref.begin(), ref.end());
    typeof(ref) xs;
    
    HAVE_MANIPULATOR(
        m1,
        once('[') >> many0(' ') >>
        maybe(read_push(xs) >> many0(many1(' ') >> read_push(xs))) >>
        maybe("...") >> "]:" >> &n);

    tm.start();
    const int REP = 1000;
    for (int i = 0; i < REP; ++ i) {
        xs.clear();
        StringReader s1(sb.c_str());
        s1 >> m1 >> sr_ends;
    }
    tm.stop();
    cout << "Elapse: " << tm.n_elapsed()/(double)REP << "ns/line" << endl
         << "Tp: " << strlen(sb.c_str())*REP*1000.0/(double)tm.n_elapsed() << "MB/s" << endl;

    xs.clear();
    StringReader s1(sb.c_str());
    s1 >> m1;
    for (size_t i = xs.size(); i < n; ++i) {
        xs.push_back(0);
    }
    ASSERT_EQ(show_container(ref), show_container(xs));
    ASSERT_TRUE(s1.good());
    ASSERT_EQ (n, xs.size());
}
    
TEST_F(test_usage_suite, read_lines)
{
    srand(time(0));
    StringWriter sb2;
    typedef std::pair<int, int> Pair;
    std::vector<Pair> ref;

    for (int i=0; i<100; ++i) {
        ref.push_back(Pair(rand(), rand()));
        sb2 << ref.back().first << "\t" << ref.back().second << "\n";
    }
    // The last line may not end with \n
    if ((rand()%100) < 50) {
        ref.push_back(Pair(rand(), rand()));
        sb2 << ref.back().first << "\t" << ref.back().second << "\n";
    }
    
    StringReader s2(sb2.c_str());
    Pair pair;
    std::vector<Pair> xs;
    HAVE_MANIPULATOR(
        read_pair,
        once(&pair.first) >> '\t' >> &pair.second>> sr_push_back(pair, xs));
    s2 >> many0(read_pair >> (once(sr_endl) | sr_ends));
    ASSERT_TRUE (s2.good());
    ASSERT_EQ (show_container(xs), show_container(ref));
}


