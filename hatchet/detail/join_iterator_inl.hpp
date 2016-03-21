// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Inline implementation of join_iterator.hpp
// Author: gejun@baidu.com
// Date: Dec.7 14:10:24 CST 2010

namespace st {
template <class _IterMakerL, class _IterTup,
          class _VarTup, class _TablePtrTup,
          class _PreFPredL>
void JoinIteratorData<_IterMakerL, _IterTup, _VarTup, _TablePtrTup, _PreFPredL>::
c_to_string (std::ostream& os)
{
    os << "{IterMakerL=" << c_show(IterMakerL)
       << " IterTup=" << c_show(IterTup)
       << " VarTup=" << c_show(VarTup)
       << " TableTup=" << c_show(TablePtrTup)
       << " PreFPredL=" << c_show(PreFPredL)
       << "}";
}

// instantiate_attr
template <class _Pos, class _IterTup, class _VarTup>
struct instantiate_attr {
    typedef typename _Pos::Attr::Type Type;
    
    static Type call (const _IterTup& iter_tup, const _VarTup&)
    { return iter_tup.template at_n<_Pos::TABLE_POS>()
            ->template at<class _Pos::Attr>(); }
};

// instantiate_var
template <class _Pos, class _IterTup, class _VarTup>
struct instantiate_var {
    typedef typename _Pos::Attr Type;
    
    static Type call (const _IterTup&, const _VarTup& var_tup)
    { return var_tup.template at_n<(-1-_Pos::TABLE_POS)>(); }
};

// instantiate_pos
template <class _Pos, class _IterTup, class _VarTup>
class instantiate_pos {
    typedef TCAP(if_, _Pos::TABLE_POS >= 0,
                 instantiate_attr<_Pos, _IterTup, _VarTup>,
                 instantiate_var<_Pos, _IterTup, _VarTup>) inst;
public:
    typedef typename inst::Type Type;
    
    static Type call (const _IterTup& iter_tup, const _VarTup& var_tup)
    { return inst::call(iter_tup, var_tup); }
};

template <class _TS, class _TSL, class _JID>
struct init_join_iterator<Cons<_TS, _TSL>, _JID> {
    static bool call (class _JID::IterTup* p_iter_tup,
                      const class _JID::VarTup& var_tup,
                      const class _JID::TablePtrTup& table_tup)
    {
        typename _JID::IterTup::template type_at_n<_TS::TABLE_POS>::R & it =
            p_iter_tup->template at_n<_TS::TABLE_POS>();

        if (!init_join_iterator<_TSL, _JID>
            ::call(p_iter_tup, var_tup, table_tup)) {
            it.set_end();
            return false;
        }

        do {
            CAP(list_at, _TS::TABLE_POS, class _JID::IterMakerL)::make(
                &it, table_tup.template at_n<_TS::TABLE_POS>(),
                *p_iter_tup, var_tup);
            for ( ; it && !check_filter_list<class _TS::FPredL,
                                             class _JID::IterTup,
                                             class _JID::VarTup>
                      ::call(*p_iter_tup, var_tup); ++ it);
        } while (!it && forward_join_iterator<_TSL, _JID>
                 ::call(p_iter_tup, var_tup, table_tup));

        return it;
    }
};

template <class _JID>
struct init_join_iterator<void, _JID> {
    static bool call (class _JID::IterTup*,
                      const class _JID::VarTup&,
                      const class _JID::TablePtrTup&)
    { return true; }
};

// forward_join_iterator
template <class _TS, class _TSL, class _JID>
struct forward_join_iterator<Cons<_TS, _TSL>, _JID> {
    static bool call (class _JID::IterTup* p_iter_tup,
                      const class _JID::VarTup& var_tup,
                      const class _JID::TablePtrTup& table_tup)
    {
        typename _JID::IterTup::template type_at_n<_TS::TABLE_POS>::R &
            it = p_iter_tup->template at_n<_TS::TABLE_POS>();
        
        ++ it;

        do {
            for ( ; it && !check_filter_list<class _TS::FPredL,
                                         class _JID::IterTup,
                                         class _JID::VarTup>
                      ::call(*p_iter_tup, var_tup); ++ it);
            if (it || !forward_join_iterator<_TSL, _JID>::call(
                    p_iter_tup, var_tup, table_tup)) {
                return it;
            }
            
            CAP(list_at, _TS::TABLE_POS, class _JID::IterMakerL)::make(
                &it, table_tup.template at_n<_TS::TABLE_POS>(),
                *p_iter_tup, var_tup);
        } while (1);
        return it;
    }
};

template <class _JID>
struct forward_join_iterator<void, _JID> {
    static bool call (class _JID::IterTup*,
                      const class _JID::VarTup&,
                      const class _JID::TablePtrTup&)
    { return false; }        
};

//JoinIterator
template <class _TSL, class _JID>
JoinIterator<_TSL, _JID>::
JoinIterator (const VarTup& var_tup, const TablePtrTup* p_table_tup)
{ init(var_tup, p_table_tup); }

template <class _TSL, class _JID>
bool JoinIterator<_TSL, _JID>::
init (const VarTup& var_tup, const TablePtrTup* p_table_tup)
{      
    var_tup_ = var_tup;
    p_table_tup_ = p_table_tup;

    if (check_filter_list<class _JID::PreFPredL, IterTup, VarTup>
        ::call(iter_tup_, var_tup)) {
        return init_join_iterator<RevTSL, _JID>
            ::call(&iter_tup_, var_tup_, *p_table_tup_);
    } else {
        set_end();
        return false;
    }
}

template <class _TSL, class _JID>
JoinIterator<_TSL, _JID>::operator bool () const
{ return iter_tup_.template at_n<LAST_TABLE_POS>(); }

template <class _TSL, class _JID>
void JoinIterator<_TSL, _JID>::set_end ()
{ iter_tup_.template at_n<LAST_TABLE_POS>().set_end(); }

template <class _TSL, class _JID>
template <template <typename> class _TBLn, class _Attr>
const typename _Attr::Type& JoinIterator<_TSL, _JID>::at () const
{ return iter_tup_.template at_n<_TBLn<_Attr>::TABLE_POS>()->at<_Attr>(); }

template <class _TSL, class _JID>
template <class _TableRep>
const typename _TableRep::Attr::Type& JoinIterator<_TSL, _JID>::at () const
{ return iter_tup_.template
        at_n<_TableRep::TABLE_POS>()->at<class _TableRep::Attr>(); }

template <class _TSL, class _JID>
void JoinIterator<_TSL, _JID>::operator++ ()
{ forward_join_iterator<RevTSL, _JID>::call(
        &iter_tup_, var_tup_, *p_table_tup_); }

template <class _TSL, class _JID>
void JoinIterator<_TSL, _JID>::c_to_string (std::ostream& os)
{
    os << "{N_ITER=" << N_ITER
       << " sizeof=" << sizeof(JoinIterator)
        //<< " JID=" << c_show(_JID)
       << "}";
}

// check_filter, 0~3 parameters
template <class _F, class _IterTup, class _VarTup>
struct check_filter<NormalizedPred<_F, void>, _IterTup, _VarTup> {
    static bool call (const _IterTup&, const _VarTup&)
    { return _F()(); }
};

template <class _F, class _Arg1, class _IterTup, class _VarTup>
struct check_filter<NormalizedPred<_F, Cons<_Arg1, void> >,
                         _IterTup, _VarTup> {
    static bool call (const _IterTup& iter_tup, const _VarTup& var_tup)
    { return _F()(instantiate_pos<_Arg1, _IterTup, _VarTup>
                  ::call(iter_tup, var_tup)); }
};

template <class _F, class _Arg1, class _Arg2,
          class _IterTup, class _VarTup>
struct check_filter<
    NormalizedPred<_F, Cons<_Arg1, Cons<_Arg2, void> > >, _IterTup, _VarTup> {
    static bool call (const _IterTup& iter_tup, const _VarTup& var_tup)
    { return _F()(instantiate_pos<_Arg1, _IterTup, _VarTup>
                  ::call(iter_tup, var_tup),

                  instantiate_pos<_Arg2, _IterTup, _VarTup>
                  ::call(iter_tup, var_tup)); }
};

template <class _F, class _Arg1, class _Arg2, class _Arg3,
          class _IterTup, class _VarTup>
struct check_filter<
    NormalizedPred<_F, Cons<_Arg1, Cons<_Arg2, Cons<_Arg3, void> > > >,
    _IterTup, _VarTup> {
    static bool call (const _IterTup& iter_tup, const _VarTup& var_tup)
    { return _F()(instantiate_pos<_Arg1, _IterTup, _VarTup>
                  ::call(iter_tup, var_tup),

                  instantiate_pos<_Arg2, _IterTup, _VarTup>
                  ::call(iter_tup, var_tup),

                  instantiate_pos<_Arg3, _IterTup, _VarTup>
                  ::call(iter_tup, var_tup)); }
};

// check_fitler_pred_list
template <class _FPred, class _FPredL, class _IterTup, class _VarTup>
struct check_filter_list<Cons<_FPred, _FPredL>, _IterTup, _VarTup> {
    static bool call (const _IterTup& iter_tup, const _VarTup& var_tup)
    { return check_filter<_FPred, _IterTup, _VarTup>::call(iter_tup, var_tup)
            && check_filter_list<_FPredL, _IterTup, _VarTup>
            ::call(iter_tup, var_tup);
    }
};

template <class _IterTup, class _VarTup>
struct check_filter_list<void, _IterTup, _VarTup>
{ static bool call (const _IterTup&, const _VarTup&) { return true; } };

}  // namespace st

