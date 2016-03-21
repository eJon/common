// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// A container gluing NamedTuple, CowHashSet and CowHashClusterSet together
// Author: gejun@baidu.com
// Date: Jan 5 21:01:44 CST 2011
#pragma once
#ifndef _COW_TABLE_HPP_
#define _COW_TABLE_HPP_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <limits>
#include <sys/stat.h>

#include "named_tuple.hpp"              // NamedTuple
#include "c_map.hpp"                    // set_include
#include "observer.hpp"                 // Observer/Event
#include "combined_tuple.hpp"           // CombinedTuple
#include "unique_index.hpp"             // UniqueIndex
#include "unique_cluster_index.hpp"     // UniqueClusterIndex
#include "detail/dump_fn.h"           // DumpFn
#include "detail/load_fn.h"           // LoadFn

namespace st {

class UniqueKeyTag {};
class ClusterKeyTag {};
class MaxFanoutTag {};

template <class _T> struct get_REVERSED { enum { R = _T::REVERSED }; };
template <class _T> struct get_MAX_FANOUT {
    static const u_int R = _T::MAX_FANOUT;
};
template <class _I> struct GetIndexPos { typedef Int<_I::POS> R; };

// Define key of a table, there're two kind of definitions:
//  1) UniqueKey<Attr1, Attr2 ... AttrN> defines a key on N distinct
//     attributes. Any two tuples in the table are different on Attr[1..N].
//     CowTable creates a hash index to speed up queries on the key.
//  2) UniqueKey<Attr1, ... AttrN, ClusterKey<Attr1' ... AttrM'> >
//     defines a key that any two tuples in the table must be different on
//     Attr[1..N] + Attr[1..M]', and tuples same on Attr[1..N] are grouped
//     and ordered by Attr[1..M]'. CowTable creates a hash_cluster index for
//     this key.
// Params: _A[0-8] are distinct attributes
// Note: _A0 can't be void because a key has at least one attribute
template <class _ArgL> class UniqueKey;

// Declare attribute set to be clustered or reversely clustered
template <class _ArgL> class ClusterKey;
template <class _ArgL> class ReversedClusterKey;

template <u_int _MAX_FANOUT> struct MaxFanout {
    typedef MaxFanoutTag Tag;
    static const u_int R = _MAX_FANOUT;
};

template <ST_SYMBOLLD9(class _P, void)> struct make_unique_key;
template <ST_SYMBOLLD9(class _P, void)> struct make_cluster_key;
template <ST_SYMBOLLD9(class _P, void)> struct make_reversed_cluster_key;
template <ST_SYMBOLLD9(class _P, void)> struct make_cow_table;

#define ST_UNIQUE_KEY(...) st::make_unique_key<__VA_ARGS__>::R
#define ST_CLUSTER_KEY(...) st::make_cluster_key<__VA_ARGS__>::R
#define ST_REVERSED_CLUSTER_KEY(...) st::make_reversed_cluster_key<__VA_ARGS__>::R
#define ST_TABLE(...)    st::make_cow_table<__VA_ARGS__>::R

template <class _CowTable> class CowTableInserter;
template <class _CowTable> class CowTableEraser;

// Insert/Erase a tuple into/from multiple indexes
// Note: at least one index and Seq
template <class _SeqL, class _IndexTup, class _Tup> struct InsertMultipleIndexes;
template <class _SeqL, class _IndexTup, class _Tup> struct EraseMultipleIndexes;

namespace ct_helper {
// Insert/Erase a tuple into multiple indexes of a table
template <class _SeqL, class _IndexTup, class _Tup> struct InsertTableAux;
template <class _SeqL, class _IndexTup, class _Tup> struct EraseTableAux;

// A function object to prepare keys for an index of a table and erase
// related tuples from multiple indexes of the same table
template <class _SeekInfo, int _Seq, class _SeqL, class _IndexTup, class _Tup>
class TableIndexEraser;

// A function object to prepare keys and seek tuples from an index of
// a table
template <class _SeekInfo, int _Seq, class _IndexTup> class TableIndexSeeker;

// Compare two index infos
// If their scores are same, returns true iff attribute set of the first index
// includes the second one
// Else returns comparison of scores
template <class _I1, class _I2> struct CompareIndexInfo;

// Convert indexed key declaration to a list of IndexInfo
template <class _PosAndIndex> class GetIndexInfo;

template <class _IndexedKey> class GetUniqueInfo;
template <class _Intersection, class _UniqueInfo> struct FoldUniqueInfo;
template <class _Uniqueness, class _UniqueInfo> struct EqUniqueness {
    static const bool R =
        CAP(set_equal, _Uniqueness, class _UniqueInfo::Uniqueness);
};

struct NotInitAux;               // Called by CowTable::not_init()
struct MemAux;                   // Called by CowTable::mem()
struct InitAux;                  // Called by CowTable::init()
struct ClearAux;                 // Called by CowTable::clear()
struct ResizeAux;                // Called by CowTable::resize(1)
struct TupleSizeAux;

}  // namespace ct_helper

// The index container
// _ArgL are attributes or key declarations(by UniqueKey<...>),
// check test/test_cow_table.cpp for examples
template <class _ArgL> class CowTable {
public:
    // Get attribute set from the argument list
    typedef TCAP(list_filter, THAP(is_tagged_with, AttributeTag), _ArgL) AttrS;
    C_ASSERT(CAP(list_size, AttrS) > 0, at_least_one_attribute);

    typedef NamedTuple<AttrS> Tup;  // Stored tuple

private:
    // Get key declarations from argument list
    typedef TCAP(list_filter, THAP(is_tagged_with, UniqueKeyTag),
                 _ArgL) KeyDeclL;
    C_ASSERT(CAP(list_size, KeyDeclL) >= 1, at_least_one_index);
    
    // Get index type from key declaration
    template <class _K> struct build_index {
        typedef TCAP(if_void,
                     typename _K::ClusterAttrS,
                     UniqueIndex<Tup, typename _K::HashAttrS>,
                     UniqueClusterIndex<Tup,
                     typename _K::HashAttrS,
                     typename _K::ClusterAttrS,
                     _K::MAX_FANOUT,
                     _K::REVERSED_CLUSTER>) R;
    };
     
public:
    // List of index types
    typedef TCAP(list_map, build_index, KeyDeclL) IndexL;

    // Get type of Nth index in this table
    template <int _N> class IndexAt : public list_at<_N, IndexL> {};

    // Number of indexes
    static const int N_INDEX = CAP(list_size, IndexL);

    typedef TCAP(list_index, IndexL) IndexM;

    // List of sorted index info, used for selection of index out of table
    typedef TCAP(list_stable_sort, ct_helper::CompareIndexInfo,
                 TCAP(list_map_cat, ct_helper::GetIndexInfo, IndexM)) IndexInfoL;
        
    // Modification sequence of indexes
    typedef TCAP(list_map, ct_helper::GetUniqueInfo, IndexM) UniqueInfoM;
    typedef TCAP(list_foldl, ct_helper::FoldUniqueInfo,
                 TCAP(list_head, UniqueInfoM)::Uniqueness,
                 TCAP(list_tail, UniqueInfoM)) CommonUniqueness;

    C_ASSERT_NOT_VOID(CommonUniqueness, indexes_should_have_common_attributes);
    typedef TCAP(set_find, CommonUniqueness, UniqueInfoM,
                 ct_helper::EqUniqueness) FirstUniqueInfo;
    C_ASSERT_NOT_VOID(FirstUniqueInfo, common_attributes_should_have_an_index_defined_on);

    static const int FirstModifyIndexPos = FirstUniqueInfo::POS;
    typedef TCAP(list_at, FirstModifyIndexPos, IndexL) FirstModifyIndex;
    typedef typename FirstModifyIndex::Pointer Pointer;
    typedef typename FirstModifyIndex::Reference Reference;

    typedef Cons<Int<FirstModifyIndexPos>,
                 TCAP(list_erase_first, Int<FirstModifyIndexPos>,
                      TCAP(list_seq, 0, N_INDEX-1))> ModifySeqL;    
private:
    typedef BasicTuple<IndexL> IndexTup;            // Tuple of indexes
    
    // position of traversal index which should be fast at traversal
    static const int FIRST_HC_POS =
        CAP(list_seek_first_true, is_unique_cluster_index, IndexL);

    static const int TRAVERSAL_INDEX_POS =
        FIRST_HC_POS >= 0 ? FIRST_HC_POS : 0;
    
    // Type of traversal index
    typedef TCAP(list_at, TRAVERSAL_INDEX_POS, IndexL) TraversalIndex;
    
public:
    // Iterator of this table
    typedef class TraversalIndex::Iterator Iterator;    

    // Nothing to in default constructor because indexes are self-constructed
    CowTable () {}

    // Copy construct from another table
    // Note: *_event are not copied
    CowTable (const CowTable& rhs) : index_tup_(rhs.index_tup_) {}
    
    // Assign from another table
    // Note: *_event are not copied
    CowTable& operator= (const CowTable& rhs)
    { 
        index_tup_ = rhs.index_tup_; 
        return *this;
    }

    // Initialize this table
    // Note: there're two alternatives to initialize a table: one is calling
    //       this method that passes same n_bucket and load_factor to all
    //       internal indexes, this is gerenally OK because suitable load
    //       factors for indexes are near values, around 60~90(I prefer 80); 
    //       and indexes automatically grow buckets due to load_factor.
    //       Another alternative is initializing index<int>() respectively, 
    //       maybe useful in some occasion.
    int init (size_t n_bucket    = CH_DEFAULT_NBUCKET,
              u_int  load_factor = CH_DEFAULT_LOAD_FACTOR)
    { return index_tup_.foldl(ct_helper::InitAux(n_bucket, load_factor), 0); }

    // Get an index at a position
    template <int _N> TCAP(list_at, _N, IndexL)& index ()
    { return index_tup_.template at_n<_N>(); }

    // Get an index at a position, const version
    template <int _N> const TCAP(list_at, _N, IndexL)& index () const
    { return index_tup_.template at_n<_N>(); }

    // Insert a tuple into this table
    // Note: this method always sends insert_event and sends replace_event
    //       if an existing tuple is replaced.
    // Returns: address of stored tuple
    Pointer insert_tuple (const Tup& tup)
    {
        return ct_helper::InsertTableAux<ModifySeqL, IndexTup, Tup>
            ::call(&index_tup_,
                   tup,
                   pre_replace_event,
                   post_replace_event, 
                   insert_event,
                   pre_insert_event);
    }    
    
    // Erase a tuple _exactly_ matching input from this table
    // Note: this method sends erase_event if the tuple is erased
    // Returns: erased or not
    bool erase_tuple (const Tup& tup)
    {
        return ct_helper::EraseTableAux<ModifySeqL, IndexTup, Tup>::call(
            &index_tup_, tup, erase_event, post_erase_event);
    }

    // Remove all tuples
    // Note: this method sends two kinds of events: clear and clear_erase,
    //       `clear' is quick and very efficient if the observer is a connector
    //       updating one table. `clear_erase' sends erasing event of all
    //       tuples, slow but useful if the observer receives from more than
    //       one signal.
    void clear ()
    {
        clear_event.notify();
        if (!clear_erase_event.empty()) {
            for (Iterator it=begin(), it_e=end(); it != it_e; ++it) {
                clear_erase_event.notify(*it);
            }
        }
        index_tup_.do_map(ct_helper::ClearAux());
    }

    int insert_by_file (const char* filename, char separator, 
                        bool abort_if_any_error = false,
                        size_t max_line = std::numeric_limits<size_t>::max());

    void resize(size_t n_bucket2)
    { index_tup_.do_map(ct_helper::ResizeAux(n_bucket2)); }
    
    // Get number of items
    size_t size() const { return index<TRAVERSAL_INDEX_POS>().size(); }

    // No item or not
    bool empty() const { return index<TRAVERSAL_INDEX_POS>().empty(); }

    // Get beginning Iterator
    Iterator begin() const { return index<TRAVERSAL_INDEX_POS>().begin(); }
    
    // Get ending Iterator
    Iterator end() const { return index<TRAVERSAL_INDEX_POS>().end(); }

    // Get memory used by this table
    // Note: `ul' is must here to avoid overflow
    size_t mem() const
    { return index_tup_.foldl(ct_helper::MemAux(), 0ul); }

    const IndexTup& index_tup () const { return index_tup_; }
    
    // Initialized or not
    bool not_init() const
    { return index_tup_.foldl(ct_helper::NotInitAux(), false); }

    // Dump table into file "filename"
    int dump(const char* dir,
             const char * filename,
             MetaData& meta_data) const
    {
        if (NULL == dir || NULL == filename) {
            ST_FATAL("file name can not be null.");
            return -1;
        }
        
        int ret = 0;
        const size_t data_size = Tup().foldl(ct_helper::TupleSizeAux(), 0);
        
        DumpFn dump_fn(data_size);
        ret = dump_fn.init(dir, filename);
        if (0 != ret) {
            ST_FATAL("fail to init DumpFn, file [%s].", filename);
            return -1;
        }

        // write meta data
        meta_data.item_size = data_size;
        meta_data.item_count = size();
        ret = dump_fn.write_meta_data(meta_data);
        if (0 != ret) {
            ST_FATAL("fail to write meta data, file [%s].", filename);
            return -1;
        }
        
        // dump table
        int error = 0;
        for (Iterator it = begin(); it != end(); ++it) {
            const Tup& tuple = *it;

            // clear buffer
            ret = dump_fn.reset();
            if (0 != ret) {
                ST_FATAL("fail to reset DumpFn, file [%s].", filename);
                error = -1;
                break;
            }
            // write tuple into buffer
            ret = tuple.foldl(dump_fn, 0);
            if (0 != ret) {
                ST_FATAL("fail to write tuple into buffer, file [%s].", filename);
                error = -1;
                break;
            }

            // write buffer into file.
            ret = dump_fn.write();
            if (0 != ret) {
                ST_FATAL("fail to write buffer, file [%s].", filename);
                error = -1;
                break;
            }
        }
        
        ret = dump_fn.close();
        if (0 != ret) {
            ST_FATAL("fail to close DumpFn, file [%s].", filename);
            error = -1;
        }

        return error != 0 ? error : 0;
    }

    // Load data from file "filename" and insert them into table
    // @param max_steps: 
    // Insert key-value into smalltable nonsequentially to save memory.
    // Smalltable use B+ tree structure to realize cluster-key.
    // Compared with other stratery, if we insert key-value sequentially, 
    // the table will cost most memory, but also be the most efficient.
    // If we need to save memory, max_steps should be greater than 1.
    // If we need to save time of "insert" process, max_steps should be 1.
    int load(const char* dir,
             const char* filename,
             const MetaData& m_data,
             const int max_steps = 1) 
    {
        if (NULL == dir || NULL == filename) {
            ST_FATAL("file name can not be null.");
            return -1;
        }
        if (0 >= max_steps) {
            ST_FATAL("max_steps must greater than zero.");
            return -1;
        }
        
        int ret = 0;
        const size_t data_size = Tup().foldl(ct_helper::TupleSizeAux(), 0);
        
        CommonMetaReader<MetaData> meta_reader;
        LoadFn load_fn(data_size);

        clear();

        ret = load_fn.init(dir, filename, &meta_reader);
        if (0 != ret) {
            ST_FATAL("fail to init LoadFn, file [%s].", filename);
            return -1;
        }
    
        // load table
        int error = 0;
        do {
            const MetaData& meta_data = meta_reader.get_meta_data();
            // validate version.
            if (meta_data.version != m_data.version) {
                ST_FATAL("meta data version [%d] is not correct, expected [%d], file [%s].",
                        m_data.version, m_data.version, filename);
                error = -1;
                break;
            }
            // validate name.
            if (strcmp(meta_data.name, m_data.name) != 0) {
                ST_FATAL("meta data name [%s] is not correct, expected [%s], file [%s].",
                        meta_data.name, m_data.name, filename);
                error = -1;
                break;
            }
            // validate partition.
            if (meta_data.partition != m_data.partition) {
                ST_FATAL("meta data partition [%d] is not correct, expected [%d], file [%s].",
                        meta_data.partition, m_data.partition, filename);
                error = -1;
                break;
            }
            // validate data size.
            if (meta_data.item_size != data_size) {
                ST_FATAL("item size [%llu] is not correct, expected [%zu], file [%s].",
                        meta_data.item_size, data_size, filename);
                error = -1;
                break;
            }
            // validate data count.
            struct stat file_info;
            bzero(&file_info, sizeof(file_info));
            std::string file_fullname(dir);
            file_fullname.append("/").append(filename);
            stat(file_fullname.c_str(), &file_info);
            
            size_t item_count = (file_info.st_size - META_HEADER_LEN) / meta_data.item_size;
            if (meta_data.item_count != item_count) {
                ST_FATAL("item count [%zu] is not correct, expected [%llu], file [%s].",
                        item_count, meta_data.item_count, filename);
                error = -1;
                break;
            }

            // load all tuples
            LoadFn::LoadState load_state;

            for (int step = 0; step < max_steps; step++) {
                if (0 != load_fn.reset()) {
                    error = -1;
                    break;
                }

                load_state = load_fn.read_next(step);
                if (LoadFn::FAIL == load_state) {
                    error = -1;
                    break;
                }

                while (!load_fn.is_end()) {
                    // init tuple with loaded data
                    Tup tuple;
                    ret = tuple.foldl(load_fn, 0);
                    if (0 != ret) {
                        ST_FATAL("fail to init tuple with loaded data, file [%s].", filename);
                        error = -1;
                        break;
                    }

                    insert(tuple);

                    // read next.
                    load_state = load_fn.read_next(max_steps);
                    if (LoadFn::FAIL == load_state) {
                        error = -1;
                        break;
                    } else if (LoadFn::END == load_state) {
                        break;
                    }
                }
                if (0 != error) {
                    break;
                }
            }
            if (0 != error) {
                break;
            }

            // validate data count.
            if (meta_data.item_count != size()) {
                ST_FATAL("item count [%llu] in meta data is not equal to tuple count in table [%zu], file [%s].",
                        meta_data.item_count, size(), filename);
                error = -1;
                break;
            }
            
        } while (0);

        ret = load_fn.close();
        if (0 != ret) {
            ST_FATAL("fail to close LoadFn, file [%s].", filename);
            error = -1;
        }
        
        return error != 0 ? error : 0;
    }

    template <class _Stream> void generic_to_string(_Stream& sw) const
    {
        sw << "CowTable"
           << "{tup=" << show_tuple_type<Tup>() << ':' << sizeof(Tup)
           << " mem=" << mem()
           << ' ' << N_INDEX << ':' << index_tup_;
        // if (!not_init()) {
        //     sw << " items=";
        //     shows_range(sw, begin(), end());
        // }
        sw << "}";
    }

    // Write to StringWriter    
    void to_string(StringWriter& sw) const
    { generic_to_string<StringWriter>(sw); }

    // Write to ostream
friend std::ostream& operator<<(std::ostream& os, const CowTable& ct)
    {
        ct.generic_to_string<std::ostream>(os);
        return os;
    }
        
    // Pack parameters as a tuple and insert into this table
    // Note: Following 9 methods should be called exclusively
#define COW_TABLE_INSERT(_n_)           \
    template <ST_SYMBOLL##_n_(typename _T)>             \
    Pointer insert (ST_PARAML##_n_(const _T, & t))      \
    { return insert_tuple(Tup(ST_SYMBOLL##_n_(t))); }

    COW_TABLE_INSERT(1)
    COW_TABLE_INSERT(2)
    COW_TABLE_INSERT(3)
    COW_TABLE_INSERT(4)
    COW_TABLE_INSERT(5)
    COW_TABLE_INSERT(6)
    COW_TABLE_INSERT(7)
    COW_TABLE_INSERT(8)
    COW_TABLE_INSERT(9);
    
private:
    template <class _SubAttrS> class FindIndexInfo {
        template <class _IndexInfo> struct equalIndexKey
        { enum { R = CAP(set_equal, _SubAttrS, class _IndexInfo::SeekInfo::KeyAttrS) }; };
    public:
        typedef TCAP(list_find_first_true, equalIndexKey, IndexInfoL) R;
    private:
        C_ASSERT (!CAP(c_void, R), no_index_on_input_attribute_set);
    };

    template <class _SubAttrS> class buildIndexEraser { 
        typedef TCAP(FindIndexInfo, _SubAttrS) II;
    public:
        typedef ct_helper::TableIndexEraser<
        class II::SeekInfo,
        II::POS,
        TCAP(list_erase_first, Int<II::POS>, ModifySeqL),
        IndexTup,
        Tup> R;
    };

    template <class _SubAttrS> class BuildIndexSeeker {
        typedef TCAP(FindIndexInfo, _SubAttrS) II;
    public:
        typedef ct_helper::TableIndexSeeker<
        class II::SeekInfo, II::POS, IndexTup> R;
    };
    
public:

    // Erase tuples matching the attribute set given in parameters
    // from this table, at most 5 parameters
    // Note: Following methods should be called exclusively
#define ST_COW_TABLE_ERASE_SET_ATTR_AUX(n) e.template set_attr<_B##n>(t##n)
#define COW_TABLE_ERASE(n)                              \
    template <ST_SYMBOLL##n(class _B)>                                  \
    bool erase (ST_PARAML##n(const typename _B, ::Type& t))             \
    {                                                                   \
        TCAP(buildIndexEraser, ST_MAKE_LIST(ST_SYMBOLL##n(_B))) e;      \
        ST_APPL##n(ST_COW_TABLE_ERASE_SET_ATTR_AUX);                    \
        return e(&index_tup_, erase_event, post_erase_event);           \
    }
    
    COW_TABLE_ERASE(1)
    COW_TABLE_ERASE(2)
    COW_TABLE_ERASE(3)
    COW_TABLE_ERASE(4)
    COW_TABLE_ERASE(5);

#define COW_TABLE_KEY_NUM(n)                                            \
    template <ST_SYMBOLL##n(class _B)>                                  \
    size_t key_num () const                                             \
    {                                                                   \
        typedef TCAP(FindIndexInfo,                                     \
                     TCAP(list_filter, c_not_void,                      \
                          ST_MAKE_LIST(ST_SYMBOLL##n(_B)))) II;         \
        return index<II::POS>().key_num(                                \
            NamedTuple<class II::SeekInfo::KeyAttrS>());                \
    }

    COW_TABLE_KEY_NUM(1)
    COW_TABLE_KEY_NUM(2)
    COW_TABLE_KEY_NUM(3)
    COW_TABLE_KEY_NUM(4)
    COW_TABLE_KEY_NUM(5);

    template <ST_SYMBOLLD5(class _B, void)> struct SeekIterator {
        typedef TCAP(BuildIndexSeeker,
                     TCAP(list_filter, c_not_void,
                          ST_MAKE_LIST(ST_SYMBOLL5(_B))))::Iterator R;
    };
    
    // Seek tuples matching the attribute set given in parameters
    // from this table, at most 5 parameters
    // Note: Following methods should be called exclusively
#define COW_TABLE_SEEK_SET_ATTR_AUX(n) s.template set_attr<_B##n>(t##n)
#define COW_TABLE_SEEK(n)                                      \
    template <ST_SYMBOLL##n(class _B)>                                  \
    TCAP(SeekIterator, ST_SYMBOLL##n(_B)) seek                          \
        (ST_PARAML##n(const typename _B, ::Type& t)) const              \
    {                                                                   \
        TCAP(BuildIndexSeeker, ST_MAKE_LIST(ST_SYMBOLL##n(_B))) s;      \
        ST_APPL##n(COW_TABLE_SEEK_SET_ATTR_AUX);                        \
        return s(index_tup_);                                           \
    }

    COW_TABLE_SEEK(1)
    COW_TABLE_SEEK(2)
    COW_TABLE_SEEK(3)
    COW_TABLE_SEEK(4)
    COW_TABLE_SEEK(5);
    
private:
    IndexTup index_tup_;
    
public:
    mutable Event<const Tup&> pre_insert_event;
    mutable Event<const Tup&> insert_event;
    mutable Event<const Tup&> erase_event;
    mutable Event<const Tup&> post_erase_event;
    mutable Event<const Tup&> pre_replace_event;
    mutable Event<const Tup&> post_replace_event;
    mutable Event<> clear_event;
    mutable Event<const Tup&> clear_erase_event;
};

}  // namespace st

#include "detail/cow_table_inl.hpp"

#endif  // _COW_TABLE_HPP_
