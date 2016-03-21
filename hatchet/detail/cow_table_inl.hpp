// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Inline parts of cow_table.hpp
// Author: gejun@baidu.com
// Date: Jan 5 21:01:44 CST 2011
namespace st {

namespace ct_helper {

// Compare two index infos
// If their scores are same, returns true iff attribute set of the first index
// includes the second one
// Else returns comparison of scores
template <class _I1, class _I2> struct CompareIndexInfo {
    static const bool R =
        (_I1::SeekInfo::SEEK_SCORE == _I2::SeekInfo::SEEK_SCORE)
        ? CAP(set_include, typename _I1::SeekInfo::KeyAttrS, typename _I2::SeekInfo::KeyAttrS)
        :(_I1::SeekInfo::SEEK_SCORE > _I2::SeekInfo::SEEK_SCORE);
};

// Called by CowTable::not_init()
struct NotInitAux {
    template <class _Index>
    bool operator() (bool not_init, const _Index& index) const
    { return not_init || index.not_init(); }
};

// Called by CowTable::mem()
struct MemAux {
    template <class _Index>
    size_t operator() (size_t mem, const _Index& index) const
    { return mem + index.mem(); }
};

// Called by CowTable::init()
struct InitAux {
    explicit InitAux (size_t n_bucket, u_int load_factor)
        : n_bucket_(n_bucket), load_factor_(load_factor)
    {}
    
    template <class _Index> int operator() (int status, _Index& index) const
    {
        // Not do initialization if initializing previous indexes was
        // already failed
        return status ? status : index.init(n_bucket_, load_factor_);
    }

private:
    size_t n_bucket_;
    u_int load_factor_;
};

// Called by CowTable::clear()
struct ClearAux {
    template <class _Index> void operator() (_Index& index) const
    { index.clear(); }
};

// Called by CowTable::resize(1)
struct ResizeAux {
    explicit ResizeAux (size_t n_bucket) : n_bucket_(n_bucket) {}
    
    template <class _Index> void operator() (_Index& index) const
    { index.resize(n_bucket_); }
private:
    size_t n_bucket_;
};

// Called by CowTable::dump/load
struct TupleSizeAux {
    template <typename T> size_t operator() (const size_t &size, T &value) const
    { return size + sizeof(value); }
};

}  // namespace ct_helper

template <ST_SYMBOLL9(class _P)> struct make_unique_key
{ typedef UniqueKey<TCAP(list_filter, c_not_void,
                         ST_MAKE_LIST(ST_SYMBOLL9(_P)))> R; };

template <ST_SYMBOLL9(class _P)> struct make_cluster_key
{ typedef ClusterKey<TCAP(list_filter, c_not_void,
                          ST_MAKE_LIST(ST_SYMBOLL9(_P)))> R; };

template <ST_SYMBOLL9(class _P)> struct make_reversed_cluster_key
{ typedef ReversedClusterKey<TCAP(list_filter, c_not_void,
                                  ST_MAKE_LIST(ST_SYMBOLL9(_P)))> R; };

template <ST_SYMBOLL9(class _P)> struct make_cow_table
{ typedef CowTable<TCAP(list_filter, c_not_void,
                        ST_MAKE_LIST(ST_SYMBOLL9(_P)))> R; };

template <class _CowTable> class CowTableInserter {
public:
    explicit CowTableInserter(_CowTable* p_table) : p_table_(p_table) {}
    
    void operator()(const class _CowTable::Tup& tup) const
    { p_table_->insert_tuple(tup); }
private:
    _CowTable* p_table_;
};

template <class _CowTable> struct CowTableEraser {
public:
    explicit CowTableEraser(_CowTable* p_table) : p_table_(p_table) {}

    void operator()(const class _CowTable::Tup& tup) const
    { p_table_->erase_tuple(tup); }

private:
    _CowTable* p_table_;
};


// -------------------
//   Implementations
// -------------------
template <class _ArgL> class ClusterKey {
    // Get MAX_FANOUT parameter
    typedef TCAP(list_find_first_true, THAP(is_tagged_with, MaxFanoutTag),
                 _ArgL) Fanout;
public:
    typedef ClusterKeyTag Tag;
    enum { REVERSED = false };
    
    static const u_int MAX_FANOUT = CAP(if_void, Fanout, 
                                        UInt<COWBASS_DEFAULT_FANOUT>, Fanout)::R;

    typedef TCAP(list_erase_first_true, THAP(is_tagged_with, MaxFanoutTag),
                 _ArgL)::Second AttrS;
};

template <class _ArgL> class ReversedClusterKey {
    // Get MAX_FANOUT parameter
    typedef TCAP(list_find_first_true, THAP(is_tagged_with, MaxFanoutTag),
                 _ArgL) Fanout;
public:
    typedef ClusterKeyTag Tag;
    enum { REVERSED = true };
    
    static const u_int MAX_FANOUT = CAP(if_void, Fanout, 
                                        UInt<COWBASS_DEFAULT_FANOUT>, Fanout)::R;

    typedef TCAP(list_erase_first_true, THAP(is_tagged_with, MaxFanoutTag),
                 _ArgL)::Second AttrS;
};

template <class _ArgL> class UniqueKey {
    typedef TCAP(list_filter, THAP(is_tagged_with, ClusterKeyTag),
                 _ArgL) ClusterL;

    C_ASSERT(CAP(list_size, ClusterL) <= 1,
             at_most_one_cluster_key_in_a_hash_key);

    typedef TCAP(list_at, 0, ClusterL) ClusterDecl;
public:
    typedef UniqueKeyTag Tag;

    // Attribute set for clustering
    typedef TCAP(if_void, ClusterDecl,
                 c_identity<void>, GetAttrS<ClusterDecl>)::R ClusterAttrS;

    // Reversed or not if has cluster
    enum { REVERSED_CLUSTER = CAP(if_void, ClusterDecl,
                                  Int<false>, get_REVERSED<ClusterDecl>)::R };

    static const u_int MAX_FANOUT = CAP(if_void, ClusterDecl,
                                        UInt<0>, get_MAX_FANOUT<ClusterDecl>)::R;
    
    // Attribute set for hashing
    typedef TCAP(list_filter, THAP(is_tagged_with, AttributeTag),
                 _ArgL) HashAttrS;
};

// Implement InsertMultipleIndexes
template <class _Seq, class _SeqL, class _IndexTup, class _Tup>
struct InsertMultipleIndexes<Cons<_Seq, _SeqL>, _IndexTup, _Tup> {
    static void call (_IndexTup* p_index_tup, const _Tup& tup)
    {
        p_index_tup->template at_n<_Seq::R>().insert(tup);
        InsertMultipleIndexes<_SeqL, _IndexTup, _Tup>::call(p_index_tup, tup);
    }
};

template <class _IndexTup, class _Tup>
struct InsertMultipleIndexes<void, _IndexTup, _Tup> {
    static void call (_IndexTup*, const _Tup&) {}
};    

// Implement EraseMultipleIndexes
template <class _Seq, class _SeqL, class _IndexTup, class _Tup>
struct EraseMultipleIndexes<Cons<_Seq, _SeqL>, _IndexTup, _Tup> {
    static bool call (_IndexTup* p_index_tup, const _Tup& tup)
    {
        // rc1/rc2 avoid possible short circuit of &&
        bool rc1 = p_index_tup->template at_n<_Seq::R>().erase_tuple(tup);
        bool rc2 = EraseMultipleIndexes<_SeqL, _IndexTup, _Tup>
            ::call(p_index_tup, tup);
        return rc1 && rc2;
    }
};

template <class _IndexTup, class _Tup>
struct EraseMultipleIndexes<void, _IndexTup, _Tup> {
    static bool call (_IndexTup*, const _Tup&) { return true; }
};


template <class _Type> struct ParseType;

template <> struct ParseType<uint8_t> {
    static uint8_t call (const char* buf, char /*sep*/, char const* * end_ptr)
    {
        long l = strtol(buf, (char**)end_ptr, 10);
        if (l > UCHAR_MAX) {
            *end_ptr = buf;
            return 0;
        }
        return static_cast<uint8_t>(l);
    }
};

template <> struct ParseType<uint16_t> {
    static uint16_t call (const char* buf, char /*sep*/, char const* * end_ptr)
    {
        long l = strtol(buf, (char**)end_ptr, 10);
        if (l > USHRT_MAX) {
            *end_ptr = buf;
            return 0;
        }
        return static_cast<uint16_t>(l);
    }
};

template <> struct ParseType<int> {
    static int call (const char* buf, char /*sep*/, char const* * end_ptr)
    {
        long l = strtol(buf, (char**)end_ptr, 10);
        if (l < INT_MIN || l > INT_MAX) {
            *end_ptr = buf;
            return 0;
        }
        return static_cast<int>(l);
    }
};

template <> struct ParseType<unsigned int> {
    static unsigned int call (const char* buf, char /*sep*/, char const* * end_ptr)
    {
        unsigned long l = strtoul(buf, (char**)end_ptr, 10);
        if (l > UINT_MAX) {
            *end_ptr = buf;
            return 0;
        }
        return static_cast<unsigned int>(l);
    }
};

template <> struct ParseType<long> {
    static long call (const char* buf, char /*sep*/, char const* * end_ptr)
    { return strtol(buf, (char**)end_ptr, 10); }
};

template <> struct ParseType<unsigned long> {
    static unsigned long call (const char* buf, char /*sep*/, char const* * end_ptr)
    { return strtoul(buf, (char**)end_ptr, 10); }
};

template <> struct ParseType<double> {
    static double call (const char* buf, char /*sep*/, char const* * end_ptr)
    { return strtod(buf, (char**)end_ptr); }
};

template <> struct ParseType<float> {
    static double call (const char* buf, char /*sep*/, char const* * end_ptr)
    { return strtof(buf, (char**)end_ptr); }
};

template <> struct ParseType<std::string> {
    static std::string call (const char* buf, char sep, char const* * end_ptr)
    {
        // Prototype is:
        // char* strchr(char*, char)
        // const char* strchr(const char*, char)
        const char* end_pos = strchr(buf, sep);
        if (end_pos == NULL) {
            *end_ptr = buf + strlen(buf);
            return std::string(buf);
        } else {
            *end_ptr = end_pos;
            return std::string(buf, (const char*)end_pos);
        }
    }
};

namespace ct_helper {
// InsertTableAux
template <class _Seq, class _SeqL, class _IndexTup, class _Tup>
class InsertTableAux<Cons<_Seq, _SeqL>, _IndexTup, _Tup> {
    typedef TCAP(_IndexTup::template type_at_n, _Seq::R) Index;
    typedef typename Index::Pointer Pointer;

public:
    static Pointer call (_IndexTup* p_index_tup,
                         const _Tup& tup,
                         const Event<const _Tup&>& pre_replace_event,
                         const Event<const _Tup&>& post_replace_event,
                         const Event<const _Tup&>& insert_event,
                         const Event<const _Tup&>& pre_insert_event)
    {
        Index* p_index = &(p_index_tup->template at_n<_Seq::R>());
        Pointer p_old_tup = p_index->seek_tuple(tup);
        // p_old_tup is probably different from tup
        if (p_old_tup) {
            const _Tup old_tup = *p_old_tup;  // cast may cost
            pre_replace_event.notify(old_tup);
            EraseMultipleIndexes<_SeqL, _IndexTup, _Tup>::call(
                p_index_tup, old_tup);
            Pointer p_tup = p_index->insert(tup);
            if (p_tup) {
                InsertMultipleIndexes<_SeqL, _IndexTup, _Tup>::call(
                    p_index_tup, tup);
                post_replace_event.notify(tup);
            }
            return p_tup;
        } else {
            // Send this event before changing indexes
            pre_insert_event.notify(tup);
            Pointer p_tup = p_index->insert(tup);
            if (p_tup) {
                InsertMultipleIndexes<_SeqL, _IndexTup, _Tup>::call(
                    p_index_tup, tup);
                insert_event.notify(tup);
            }
            return p_tup;
        }
    }
};

template <class _Seq, class _IndexTup, class _Tup>
class InsertTableAux<Cons<_Seq, void>, _IndexTup, _Tup> {
    typedef TCAP(_IndexTup::template type_at_n, _Seq::R) Index;
    typedef typename Index::Pointer Pointer;
public:
    static Pointer call (_IndexTup* p_index_tup,
                         const _Tup& tup,
                         const Event<const _Tup&>& pre_replace_event,
                         const Event<const _Tup&>& post_replace_event,
                         const Event<const _Tup&>& insert_event,
                         const Event<const _Tup&>& pre_insert_event)
    {
        Index* p_index = &(p_index_tup->template at_n<_Seq::R>());
        
        if (pre_replace_event.empty() &&
            post_replace_event.empty() &&
            pre_insert_event.empty() &&
            insert_event.empty()) {
            return p_index->insert(tup);
        }

        Pointer p_old_tup = p_index->seek_tuple(tup);
        if (p_old_tup) {  // Replace an old tuple
            if (!pre_replace_event.empty()) {
                pre_replace_event.notify(*p_old_tup);
            }
            Pointer p_tup = p_index->insert(tup);
            if (p_tup && !post_replace_event.empty()) {
                post_replace_event.notify(*p_tup);
            }
            return p_tup;
        } else {  // Insert a new tuple
            pre_insert_event.notify(tup);
            Pointer p_tup = p_index->insert(tup);
            if (p_tup && !insert_event.empty()) {
                insert_event.notify(*p_tup);
            }
            return p_tup;
        }
    }
};

// EraseTableAux
template <class _Seq, class _SeqL, class _IndexTup, class _Tup>
class EraseTableAux<Cons<_Seq, _SeqL>, _IndexTup, _Tup> {
    typedef TCAP(_IndexTup::template type_at_n, _Seq::R) Index;
    typedef typename Index::Pointer Pointer;
public:
    static bool call (_IndexTup* p_index_tup,
                      const _Tup& tup,
                      const Event<const _Tup&>& erase_event,
                      const Event<const _Tup&>& post_erase_event)
    {
        Index* p_index = &(p_index_tup->template at_n<_Seq::R>());
        Pointer p_old_tup = p_index->seek_tuple(tup);
        if (p_old_tup && tup == (*p_old_tup)) {
            erase_event.notify(tup);  // Send event first
            EraseMultipleIndexes<_SeqL, _IndexTup, _Tup>::call(
                p_index_tup, tup);
            p_index->erase_tuple(tup);

            // Note: this is OK because "*p_old_tup == tup"
            post_erase_event.notify(tup);
            return true;
        }
        return false;  // match nothing
    }
};

template <class _Seq, class _IndexTup, class _Tup>
class EraseTableAux<Cons<_Seq, void>, _IndexTup, _Tup> {
    typedef TCAP(_IndexTup::template type_at_n, _Seq::R) Index;
    typedef typename Index::Pointer Pointer;
    
public:
    static bool call (_IndexTup* p_index_tup,
                      const _Tup& tup,
                      const Event<const _Tup&>& erase_event,
                      const Event<const _Tup&>& post_erase_event)
    {
        Index* p_index = &(p_index_tup->template at_n<_Seq::R>());
        Pointer p_old_tup = p_index->seek_tuple(tup);
        if (p_old_tup && tup == (*p_old_tup)) {
            erase_event.notify(tup);  // Send event first
            p_index->erase_tuple(tup);

            // Note: this is OK because "*p_old_tup == tup"
            post_erase_event.notify(tup);
            return true;
        }
        return false;
    }
};

// Note: caller should guarantee that keys are fully initialized using
//       `set_attr' before `operator()'
template <class _SeekInfo, int _Seq, class _SeqL, class _IndexTup, class _Tup>
class TableIndexEraser {
    typedef class _IndexTup::template type_at_n<_Seq>::R Index;
    typedef NamedTuple<class _SeekInfo::KeyAttrS> Key;

public:
    bool operator() (_IndexTup* p_index_tup,
                     const Event<const _Tup&>& erase_event,
                     const Event<const _Tup&>& post_erase_event) const
    {
        Index & index = p_index_tup->template at_n<_Seq>();
        typename _SeekInfo::SeekIterator it = index.seek(key_);
        size_t n = 0;

        while (it) {
            const _Tup tup = *it;
            ++ it;
            ++ n;

            if (it) {  // not last element
                erase_event.notify(tup);
                EraseMultipleIndexes<_SeqL, _IndexTup, _Tup>::call(
                    p_index_tup, tup);
            } else {
                if (1 == n) {
                    erase_event.notify(tup);
                    EraseMultipleIndexes<_SeqL, _IndexTup, _Tup>::call(
                        p_index_tup, tup);
                    index.erase(key_);
                } else {  // n > 1
                    // FIXME(gejun): A dirty hack to force the chain contain one
                    // element before sendinng last erase_event, which let
                    // observers know the chain is about to be deleted
                
                    // Dirty: Erase the chain and insert last tuple back
                    index.erase(key_);
                    index.insert(tup);
                    erase_event.notify(tup);
                    EraseMultipleIndexes<_SeqL, _IndexTup, _Tup>::call(
                        p_index_tup, tup);
                    index.erase(key_);
                }

                if (!post_erase_event.empty()) {
                    post_erase_event.notify(tup);
                }
                return true;
            }
        }
        return false;
    }

    template <class _Attr> void set_attr (const typename _Attr::Type& v)
    { key_.template at<_Attr>() = v; }

private:
    Key key_;
};

// Note: caller should guarantee that keys are fully initialized using
//       `set_attr' before `operator()'
template <class _SeekInfo, int _Seq, class _IndexTup>
class TableIndexSeeker {
public:
    typedef class _IndexTup::template type_at_n<_Seq>::R Index;
    typedef NamedTuple<class _SeekInfo::KeyAttrS> Key;
    typedef class _SeekInfo::SeekIterator Iterator; 

    Iterator operator() (const _IndexTup& index_tup) const
    { return index_tup.template at_n<_Seq>().seek(key_); }

    template <class _Attr> void set_attr (const typename _Attr::Type& v)
    { key_.template at<_Attr>() = v; }

private:
    Key key_;
};

template <class _PosAndIndex> class GetUniqueInfo {
public:
    typedef UniqueInfo<_PosAndIndex::First::R,
                      typename _PosAndIndex::Second::Uniqueness> R;
};

template <class _Intersection, class _UniqueInfo> struct FoldUniqueInfo {
    typedef TCAP(set_intersect, _Intersection, class _UniqueInfo::Uniqueness) R;
};

template <class _PosAndIndex> class GetIndexInfo {
    template <class _SeekInfo>
    struct SeekInfo2IndexInfo {
        typedef IndexInfo<_PosAndIndex::First::R, _SeekInfo> R;
    };
public:
    typedef TCAP(list_map, SeekInfo2IndexInfo, class _PosAndIndex::Second::SeekInfoL) R;
};

template <class _Tup, class _AttrL> struct ParseTupleAux;

template <class _Tup, class _Attr, class _AttrL>
struct ParseTupleAux<_Tup, Cons<_Attr, _AttrL> > {
    static int call (_Tup* p_tup, const char* buf, char sep)
    {
        const char* end_ptr = buf;
        typename _Attr::Type val = ParseType<typename _Attr::Type>::call(buf, sep, &end_ptr);
        if (end_ptr > buf) {
            p_tup->template at<_Attr>() = val;
            for ( ; end_ptr && *end_ptr != '\0' && *end_ptr == sep; ++end_ptr);
            return ParseTupleAux<_Tup, _AttrL>::call(p_tup, end_ptr, sep);
        } else {
            return -1;
        }
    }
};

template <class _Tup> struct ParseTupleAux<_Tup, void> {
    static int call (_Tup*, const char* buf, char)
    {
        return (buf == NULL || *buf == '\0') ? 0 : -1;
    }
};

}  // namespace ct_helper

template <class _ArgL>
int CowTable<_ArgL>::insert_by_file (
    const char* filename, char sep, bool abort_if_any_error, size_t max_line)
{
    FILE* fp = fopen(filename, "r");
    if (NULL == fp) {
        ST_FATAL("Fail to open `%s'", filename);
        return -1;
    }

    char* buf = NULL;
    size_t buf_len = 0;
    Tup tup;
    int rc = 0;
    size_t line_no = 1;
    // From `man getline': On success, getline() and getdelim() return
    // the number of characters read, including the delimiter character,
    // but not including the terminating null character
    for (ssize_t n_read = 0;
         (n_read = getline(&buf, &buf_len, fp)) != -1;
         ++line_no) {
        if (line_no > max_line) {
            ST_WARN("exceed the max line limit %zu. abort. FileName:`%s'", max_line, filename);
            break;
        }
        
        if (buf[n_read-1] == '\r' || buf[n_read-1] == '\n') {
            buf[--n_read] = '\0';
        }
        if (buf[n_read] == '\0' &&  // ended with zero
            0 == ct_helper::ParseTupleAux<Tup, AttrS>::call(&tup, buf, sep)) {
            insert_tuple(tup);
        } else if (abort_if_any_error) {
            rc = -1;
            ST_FATAL("Fail to parse line %zu: \"%s\", abort. FileName:`%s'", line_no, buf, filename);
            break;
        } else {
            rc = 1;
            ST_WARN("Fail to parse line %zu: \"%s\". FileName:`%s'", line_no, buf, filename);
        }
    }

    // Clear resources
    if (fp) {
        fclose(fp);
    }
    if (buf) {
        free(buf);
    }
    return rc;
}


// type printings
template <class _ArgL> struct c_show_impl<UniqueKey<_ArgL> > {
    static void c_to_string(std::ostream& os)
    { os << "UniqueKey" << c_show(_ArgL); }
};

template <class _ArgL> struct c_show_impl<ClusterKey<_ArgL> > {
    static void c_to_string(std::ostream& os)
    { os << "ClusterKey" << c_show(_ArgL); }
};

template <class _ArgL> struct c_show_impl<ReversedClusterKey<_ArgL> > {
    static void c_to_string(std::ostream& os)
    { os << "ReversedClusterKey" << c_show(_ArgL); }
};

template <class _ArgL> struct c_show_impl<CowTable<_ArgL> > {
    static void c_to_string(std::ostream& os)
    { os << "CowTable" << c_show(_ArgL); }
};


}  // namespace st

