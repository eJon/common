// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Author: gejun@baidu.com
// Date: 2010-12-04 11:59

#include <gtest/gtest.h>
#include "functional.hpp"
#include "st_timer.h"
#include "default_value.h"
#include "choose_variadic.h"

#define dprint0() printf("hello0")
#define dprint1(_1) printf("hello1")
#define dprint2(_1,_2) printf("hello2")
#define dprint(...) CHOOSE_VARIADIC_MACRO(dprint, __VA_ARGS__)

using namespace st;

// template <template <typename> class _F1, template <typename> class _F2>
// struct f1_con {
//     template <typename _T>
//     struct HR {
//         typedef typename _F1<typename _F2<_T>::R>::R R;
//     };
// };

//     // apply an argument to a template class with one parameters
//     template <template <typename> class _F, typename _A1>
//     struct f1_apply { typedef _F<_A1> HR; };

// apply an argument to a template class with two parameters

// template <template <typename, typename> class _F, typename _A1>
// struct f_apply {
//     template <typename _A2> struct HR
//     { typedef TCAP(_F, _A1, _A2) R; };
// };

#define C_CONNECT(_fun_, _gun_)                                 \
    c_identity<typeof(c_connect_foo<_fun_, _gun_>())>::R::HR

template <template <typename> class _F, template <typename> class _G>
struct c1_connect_helper {
    template <typename _A1> struct HR
    { typedef TCAP(_F, TCAP(_G, _A1)) R; };
};

template <template <typename> class _F, template <typename> class _G>
extern c1_connect_helper<_F, _G> c_connect_foo();

template <template <typename> class _F,
          template <typename, typename> class _G>
struct c2_connect_helper {
    template <typename _A1, typename _A2> struct HR
    { typedef TCAP(_F, TCAP(_G, _A1, _A2)) R; };
};

template <template <typename> class _F,
          template <typename, typename> class _G>
extern c2_connect_helper<_F, _G> c_connect_foo();


#define PAP(_fun_, _arg_)                               \
    c_identity<typeof(pap_foo<_fun_, _arg_>())>::R::HR

#define PAPN(_fun_, _arg_)                              \
    c_identity<typeof(papn_foo<_fun_, _arg_>())>::R::HR

template <template <typename> class _F, typename _A1>
struct pap1_helper
{ struct HR { typedef TCAP(_F, _A1) R; }; };

template <template <typename> class _F, typename _A1>
extern pap1_helper<_F, _A1> pap_foo();

template <template <typename> class _F, typename _A1>
struct papn1_helper
{ struct HR { enum { R = CAP(_F, _A1) }; }; };

template <template <typename> class _F, typename _A1>
extern papn1_helper<_F, _A1> papn_foo();


template <template <typename, typename> class _F, typename _A1>
struct pap2_helper {
    template <typename _A2> struct HR
    { typedef TCAP(_F, _A1, _A2) R; };
};

template <template <typename, typename> class _F, typename _A1>
extern pap2_helper<_F, _A1> pap_foo();


template <template <template <typename> class _G, typename> class _F,
          template <typename> class _A1>
struct papf2_helper {
    template <typename _A2> struct HR
    { typedef TCAP(_F, _A1, _A2) R; };
};

template <template <template <typename> class _G, typename> class _F,
          template <typename> class _A1>
extern papf2_helper<_F, _A1> pap_foo();


template <template <typename, typename> class _F, typename _A1>
struct papn2_helper {
    template <typename _A2> struct HR
    { enum { R = CAP(_F, _A1, _A2) }; };
};

template <template <typename, typename> class _F, typename _A1>
extern papn2_helper<_F, _A1> papn_foo();

// template <template <typename> class _F>
// struct c1_hofun_helper {
//     template <typename _A1> struct F {
//         typedef TCAP(F, _A1) R;
//     };
// };

// template <template <typename> class _F>
// c1_hofun_helper<_F> extern c_hofun_foo();

// template <template <typename, typename> class _F>
// struct c2_hofun_helper {
//     typedef _F F;
// };

// template <template <typename, typename> class _F>
// c2_hofun_helper<_F> extern c_hofun_foo();



extern int return_foo ();

struct ReturnFoo {
    enum { R = sizeof(return_foo()) };
};

int main(int argc, char **argv)
{
    cout << ReturnFoo::R << endl;
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

template <typename _List, typename _E>
struct reverse_store {
    typedef Cons<_E, _List> R;
};

template <typename _T>
struct DummyFilter {
    bool operator() (_T x, _T y) const
    { return x > y; }
};

template <typename _Pt1, typename _Pt2>
struct compare_pt {
    static const bool R = (_Pt1::X < _Pt2::X);
};

struct Foo {};

struct Foo2 : public Foo {};

struct Foo3 : public Foo2 {};

struct Goo {
    int x;
};

std::ostream& operator<< (std::ostream& os, const Goo&)
{ return os << "bingo"; }


class LogStream {
    typedef ostream& (*StreamOp)(ostream&);

public:
    explicit LogStream ()
    {
        len_ = 32;
        buf_ = ST_NEW_ARRAY(char, len_);
        oss_.rdbuf()->pubsetbuf(buf_, len_);
    }
    
    template <typename _T> LogStream& operator<< (_T t)
    {
        oss_ << t;
        return *this;
    }

    
    LogStream& operator<< (StreamOp pf)
    {
        if (pf == static_cast<StreamOp>(std::endl)) {
            flush();
        } else {
            pf(oss_);
        }
        return *this;
    }

    void flush ()
    {
        if (oss_.tellp() < len_) {
            buf_[oss_.tellp()] = '\0';
        }
        ul_writelog(UL_LOG_NOTICE, oss_.str().c_str());
        //cout << "[flushed]" << oss_.str() << "[flushend]" << endl;
        //bzero(buf, sizeof(buf));

        cout << "state=" << oss_.rdstate()
             << " p=" << oss_.tellp()
             << endl;

        oss_.seekp(0);
    }
    ostringstream oss_;
    char* buf_;
    int len_;
};


TEST_F(test_usage_suite, log_stream)
{
    dprint(2,3);
    
    cout << ST_DEFAULT_VALUE_OF(int) << endl
         << ST_DEFAULT_VALUE_OF(short) << endl;
    
    LogStream ls;
    ls << Goo();
    ls.buf_[0] = 'A';
    ls << endl;
    ls << "hello" << "there";
    ls.buf_[0] = 'A';
    ls << endl;
    ls << "hellofdasjfkdjafdsafffffffffffffffffffffffffffff;lsakjfkdjsalfdjasfd";
    ls.buf_[0] = 'A';
    ls << endl;
    ls << hex << 256;
    ls.buf_[0] = 'A';
    ls << endl;
}

TEST_F(test_usage_suite, check_inheritance)
{
    cout << CAP(is_base_of, Foo, Foo2) << endl;
    cout << CAP(is_base_of, Foo, Foo3) << endl;
    cout << CAP(is_base_of, Foo2, Foo3) << endl;
    cout << CAP(is_base_of, Foo3, Foo2) << endl;    
}

struct customized_show : public c_show_base {
    static void c_to_string (std::ostream& os) {
        os << "Hello world";
    }
};


template <int _X, int _Y> struct CPoint
    : public c_show_as<Char<'('>, Int<_X>, Char<','>,
                       Char<'/'>, Int<_Y>, Char<')'> > {};

//c_show_as<_Pos, _1, "Hello world", Char<'('>ace, Po>

TEST_F(test_usage_suite, compile_time_show)
{
    cout << c_show(Int<1>)
         << c_show(Char<','>)
         << c_show(Char<'('>)
         << c_show(Int<2>)
         << c_show(int)
         << c_show(Char<')'>)
         << c_show_multiple(Char<'('>, Int<1>, Int<2>, customized_show, Char<')'>)
        //<< c_show(Cons<_1,Cons<_2,void > > )
        //<< c_show(CPoint<49, 23>)
         << endl;
}

struct check_bit_op {
    //! check bit_num
    C_ASSERT (bit_num<1>::R == 1,  bit_num_of_1_is_not_1);
    C_ASSERT (bit_num<2>::R == 2,  bit_num_of_2_is_not_2);
    C_ASSERT (bit_num<31>::R == 5,  bit_num_of_31_is_not_5);
    C_ASSERT (bit_num<32>::R == 6,  bit_num_of_32_is_not_6);
    C_ASSERT (bit_num<33>::R == 6,  bit_num_of_33_is_not_6);


    //! check bit_shift
    C_ASSERT (bit_shift<1>::R == 0,  bit_shift_of_1_is_not_0);
    C_ASSERT (bit_shift<2>::R == 1,  bit_shift_of_2_is_not_1);
    C_ASSERT (bit_shift<4>::R == 2,  bit_shift_of_4_is_not_2);
    C_ASSERT (bit_shift<31>::R == 5,  bit_shift_of_31_is_not_5);
    C_ASSERT (bit_shift<32>::R == 5,  bit_shift_of_32_is_not_5);
    C_ASSERT (bit_shift<33>::R == 6,  bit_shift_of_33_is_not_6);
};

TEST_F(test_usage_suite, higher_order_templates)
{
    typedef CAP(make_list, Int<1>,Int<2>,Int<0>,Int<3>,Int<4>,
                Int<0>,Int<5>,Int<2> ) NumL;
    cout << "list1=" << c_show(NumL) << endl;
    //cout << CAP(list_filter, F_APPLY(c_same, Int<2>), NumL) << endl;
    //typedef FN_APPLY(c_same, Int<2>) ft;
    cout << c_show(CAP(PAP(list_filter, PAPN(c_not_same, Int<2>)), NumL))
         << endl;
    cout << c_show(CAP(list_filter, PAPN(c_not_same, Int<2>), NumL)) << endl;
}

template <int _X, int _Y> struct Vector2I {
    static const int X = _X;
    static const int Y = _Y;
};
namespace st {
// convert Vector2I to std::string
template <int _X, int _Y>
struct c_show_impl<Vector2I<_X, _Y> > {
    static void c_to_string (std::ostream& os)
    { os << "_(" << _X << ' ' << _Y << ')'; }
};
}

TEST_F(test_usage_suite, list_operations)
{
    typedef CAP(make_list, Vector2I<20, 10>, Vector2I<18, 3>,
                Vector2I<18, 2>, Vector2I<20, 12>, Vector2I<1, 20>,
                Vector2I<1, 30>) UnorderedList;
    cout << "before_sort=" << c_show(UnorderedList) << endl;
    cout << "after_sort=" << c_show(CAP(list_stable_sort, compare_pt, UnorderedList)) << endl;
   
    
    typedef CAP(map_insert, Int<1>, short, CAP(map_insert, Int<2>, long, CAP(map_insert, Int<3>, int, void))) m1;
    
    //cout << c_show<map_insert<Int<1>, int, void>::R>::R() << endl;
    cout << "size of m1=" << CAP(list_size, m1) << endl;
    C_ASSERT(CAP(c_same, short, CAP(map_find, Int<1>, m1)), bad_map_find);
    C_ASSERT(CAP(c_same, long, CAP(map_find, Int<2>, m1)), bad_map_find);
    C_ASSERT(CAP(c_same, int, CAP(map_find, Int<3>, m1)), bad_map_find);
    C_ASSERT(CAP(c_same, void, CAP(map_find, Int<4>, m1)), bad_map_find);
    cout << "m1=" << c_show(m1) << endl;

    typedef CAP(map_insert, Int<2>, char, m1) m2;
    cout << "size of m2=" << CAP(list_size, m2) << endl;
    C_ASSERT(CAP(c_same, short, CAP(map_find, Int<1>, m2)), bad_map_find);
    C_ASSERT(CAP(c_same, char, CAP(map_find, Int<2>, m2)), bad_map_find);
    C_ASSERT(CAP(c_same, int, CAP(map_find, Int<3>, m2)), bad_map_find);
    C_ASSERT(CAP(c_same, void, CAP(map_find, Int<4>, m2)), bad_map_find);

    cout << "m2=" << c_show(m2) << endl;
    
    C_ASSERT(CAP(map_find_pos, Int<4>, m2) < 0, user_id_should_not_exist);
    C_ASSERT(CAP(map_find_pos, Int<2>, m2) == 0, site_id_is_not_at_1);
    C_ASSERT(CAP(map_find_pos, Int<1>, m2) == 1, plan_id_is_not_at_0);
    C_ASSERT(CAP(map_find_pos, Int<3>, m2) == 2, unit_id_is_not_at_2);


    typedef CAP(map_erase, Int<2>, m2) m3;
    cout << "size of m3=" << CAP(list_size, m3) << endl;
    C_ASSERT((c_same<short, map_find<Int<1>, m3>::R>::R), bad_map_find);
    C_ASSERT(CAP(c_same, void, CAP(map_find, Int<2>, m3)), bad_map_find);
    C_ASSERT((c_same<int, map_find<Int<3>, m3>::R>::R), bad_map_find);
    C_ASSERT((c_same<void, map_find<Int<4>, m3>::R>::R), bad_map_find);

    //correct_header_traits::duplicated_attr;
    typedef CAP(make_list, short, float, int, long) l1;
    cout << "orig=" << c_show(l1) << endl;
    cout << "reve=" << c_show(CAP(list_foldl, reverse_store, void, l1)) << endl;
    C_ASSERT (CAP(c_same, CAP(list_at, 0, l1), short), bad_type);
    C_ASSERT (CAP(c_same, CAP(list_at, 1, l1), float), bad_type);
    C_ASSERT (CAP(c_same, CAP(list_at, 2, l1), int), bad_type);
    C_ASSERT (CAP(c_same, CAP(list_at, 3, l1), long), bad_type);
    C_ASSERT (CAP(c_same, CAP(list_at, 4, l1), void), bad_type);

    C_ASSERT (CAP(list_size, l1) == 4, length_of_l1_is_not_4);
    C_ASSERT (!CAP(c_same, short, long), short_st_andlong_is_not_same);
    C_ASSERT (CAP(c_same, long, long), should_be_same);
    C_ASSERT (2 == CAP(list_seek_first, int, l1), should_be_2);
    C_ASSERT (3 == CAP(list_seek_first, long, l1), should_be_3);
    C_ASSERT (!CAP(list_dup, l1), no_dup);

    typedef CAP(list_seq, 2, 10) seq1;
    typedef CAP(list_seq, 20, 40) seq2;
    cout << "seq1=" << c_show(seq1) << endl;
    cout << "seq2=" << c_show(seq2) << endl;
    cout << "zip2(seq1, seq2)=" << c_show(CAP(list_zip2, seq1, seq2)) << endl;

    
    //cout << c_show<filter<is_assign, p1>::R>::R() << endl;
    cout << "list_erase_first=" << c_show(CAP(list_erase_first, float, l1)) << endl;

    C_ASSERT (CAP(c_same, CAP(list_split, -3, l1),
                  Pair<void, l1>), bad);
    C_ASSERT (CAP(c_same, CAP(list_split, 0, l1),
                  Pair<void, l1>), bad);
    C_ASSERT (CAP(c_same, CAP(list_split, 1, l1),
                  Pair<Cons<short,void>, CAP(make_list, float, int, long)>), bad);
    C_ASSERT (CAP(c_same, CAP(list_split, 2, l1),
                  Pair<CAP(make_list,short,float), CAP(make_list, int, long)>), bad);
    C_ASSERT (CAP(c_same, CAP(list_split, 3, l1),
                  Pair<CAP(make_list, short, float, int), CAP(make_list, long)>), bad);
    C_ASSERT (CAP(c_same, CAP(list_split, 4, l1),
                  Pair<l1, void>), bad);
    C_ASSERT (CAP(c_same, CAP(list_split, 5, l1),
                  Pair<l1, void>), bad);
}

template <bool> struct case1;

template <> struct case1<true> {
    typedef int R;
};

template <bool> struct case2;

template <> struct case2<false> {
    typedef int R;
};

struct should_compile {
    enum { b = 0 };
    typedef CAP(if_, b, case1<b>, case2<b>)::R R;
};

TEST_F(test_usage_suite, test_lazy)
{
    should_compile::R x __attribute__((unused));
}

struct Undefined;

template <typename _T> struct has_positive_value
{ enum { R = _T::R > 0 }; };

template <template <typename> class _F, typename _L>
struct eager_list_all { enum { R = false }; };

//! pattern match _L as void
template <template <typename> class _F>
struct eager_list_all<_F, void> { enum { R = true }; };

//! pattern match _L
template <template <typename> class _F, typename _H, typename _T>
struct eager_list_all<_F, Cons<_H, _T> >
{ enum { R = CAP(_F, _H) && CAP(eager_list_all, _F, _T) }; };

TEST_F(test_usage_suite, lazy_list_all)
{
    //can compile
    cout << CAP(list_all, has_positive_value,
                CAP(make_list, Int<1>, Int<0>, Int<3>, Undefined)) << endl;

    // can't compile
    //cout << CAP(eager_list_all, has_positive_value, make_list<Int<1>, Int<0>, Int<3>, Undefined>::R) << endl;
}

struct C1 {
    struct C2;
    C2 begin () const;
};

struct C1::C2 {
    int x;
    int y;
};

C1::C2 C1::begin () const
{ return C1::C2(); }

TEST_F(test_usage_suite, test_hash)
{
    cout << "sizeof(C1)=" << sizeof(C1) << endl;
    
    ASSERT_EQ (sizeof(size_t), sizeof(size_t));

    // We use std::hash instead
    // ASSERT_EQ (size_t('h'), hash("h"));
    // ASSERT_EQ (size_t('h')*5 + size_t('e'), hash("he"));
    // ASSERT_EQ (size_t('h')*5*5 + size_t('e')*5 + size_t('l'),
    //            hash("hel"));
    ASSERT_EQ (13ul, hash(13));
}

TEST_F(test_usage_suite, test_default_value_of_primitives)
{
    ASSERT_EQ(char(), 0);
    ASSERT_EQ(short(), 0);
    ASSERT_EQ(int(), 0);
    ASSERT_EQ(long(), 0l);
    typedef long long longlong_t;
    ASSERT_EQ(longlong_t(), 0ll);
}


// struct VAR2 {};

// #define stpred(_fo_, ...)  CConj<Cons<CPred<_fo_, ##__VA_ARGS__>, void> >()
// #define TB(_attr_,_tseq_) IndexedAttr<_tseq_, _attr_>()
// #define TB1(_attr_) IndexedAttr<1, _attr_>()
// #define TB2(_attr_) IndexedAttr<2, _attr_>()
// #define TB3(_attr_) IndexedAttr<3, _attr_>()
// #define ST_select2(_t1_, _t2_) Cons<typeof(_t1_), Cons<typeof(_t2_), void> >
// #define stwhere(_cond_) typeof(_cond_)
// #define stvar(_type_) VAR2

// namespace st {
//     template <typename _PredL> struct CConj {
//         typedef _PredL PredL;

//         template <typename _Conj2>
//         CConj<TCAP(list_concat, PredL, typename _Conj2::PredL)>
//         operator&& (_Conj2) const;
//     };

//     template <typename _IA1, typename _IA2> struct CEqual {};
//     template <typename _IA1, typename _IA2> struct CLess {};
//     template <typename _IA1, typename _IA2> struct CLessEqual {};
//     template <typename _IA1, typename _IA2> struct CNotEqual {};
//     template <typename _IA1, typename _IA2> struct CGreaterEqual {};
//     template <typename _IA1, typename _IA2> struct CGreater {};


//     template <int _TSEQ, typename _Attr> struct IndexedAttr {
//         static const int TSEQ = _TSEQ;

//         typedef _Attr Attr;
    
//         template <typename _IA2>
//         CConj<Cons<CEqual<IndexedAttr, _IA2>, void> > operator== (_IA2) const;

//         template <typename _IA2>
//         CConj<Cons<CLess<IndexedAttr, _IA2>, void> > operator< (_IA2) const;

//         template <typename _IA2>
//         CConj<Cons<CLessEqual<IndexedAttr, _IA2>, void> > operator<= (_IA2) const;

//         template <typename _IA2>
//         CConj<Cons<CNotEqual<IndexedAttr, _IA2>, void> > operator!= (_IA2) const;

//         template <typename _IA2>
//         CConj<Cons<CGreaterEqual<IndexedAttr, _IA2>, void> > operator>= (_IA2) const;

//         template <typename _IA2>
//         CConj<Cons<CGreater<IndexedAttr, _IA2>, void> > operator> (_IA2) const;
//     };

//     template <typename _T> struct extract_pred_arg;

//     template <typename _Ret> struct extract_pred_arg<_Ret()>
//     { typedef _Ret R; };

//     template <> struct extract_pred_arg<VAR2>
//     { typedef VAR2 R; };

//     template <typename _F, typename _A1=void, typename _A2=void, typename _A3=void>
//     struct CPred {
//         typedef _F Fun;
//         typedef TCAP(list_map, extract_pred_arg,
//                      TCAP(list_filter, c_not_void,
//                           Cons<_A1, Cons<_A2, Cons<_A3, void> > >)) ArgL;
//     };

//     template <typename _L>
//     struct c_show_impl<CConj<_L> > {
//         static void c_to_string(std::ostream& os) {
//             os << "Conj=" << c_show(_L);
//         }
//     };

//     template <typename _T1, typename _T2>
//     struct c_show_impl<CEqual<_T1, _T2> > {
//         static void c_to_string(std::ostream& os)
//         { os << "(" << c_show(_T1) << " == " << c_show(_T2) << ")"; }
//     };

//     template <typename _T1, typename _T2>
//     struct c_show_impl<CGreaterEqual<_T1, _T2> > {
//         static void c_to_string(std::ostream& os)
//         { os << "(" << c_show(_T1) << " >= " << c_show(_T2) << ")"; }
//     };

    
//     template <int _TSEQ, typename _A>
//     struct c_show_impl<IndexedAttr<_TSEQ, _A> > {
//         static void c_to_string(std::ostream& os) {
//             os << c_show(_A) << '(' << _TSEQ << ')';
//         }
//     };

//     template <typename _F, typename _A1, typename _A2, typename _A3>
//     struct c_show_impl<CPred<_F, _A1, _A2, _A3> > {
//         typedef CPred<_F, _A1, _A2, _A3> UP;
//         static void c_to_string(std::ostream& os) {
//             os << c_show(_F) << "(" << c_show(typename UP::ArgL) << ")";
//         }
//     };
// }

// struct UNIT_ID {};
// struct PLAN_ID {};

// TEST_F(test_usage_suite, fancy_ct_predicates)
// {
//     cout << c_show(stwhere(TB1(UNIT_ID) == TB2(UNIT_ID)
//                            && TB1(Int<2>) == TB2(Int<2>)
//                            && TB1(Int<3>) >= TB2(Int<1>)
//                            && stpred(DummyFilter<int>, TB1(Int<3>), stvar(int))
//                            && stpred(DummyFilter<float>))) << endl;
// }


