// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Provide a table that supports runtime schema
// Author: gejun@baidu.com
// Date: Jun 22 17:21:40 CST 2011
#pragma once
#ifndef _DYN_TABLE_H_
#define _DYN_TABLE_H_

#include "dyn_index.h"             // DynIndex

namespace st {

// Pack parameters as a DynTuple and insert into this table
// Returns: address of inserted tuple, NULL means failure
// Note: Following 9 methods should be called exclusively
#define DYN_TABLE_INSERT_SET_N(n) tup.at_n(n) = (t##n)
    
#define HAVE_DYN_TABLE_INSERT(_n_)                                      \
    template <ST_SYMBOLL##_n_(typename _T)>                             \
    void* insert(ST_PARAML##_n_(const _T, & t))                         \
    {                                                                   \
        if (_n_ != schema_.field_num()) {                               \
            ST_FATAL("Parameters(%d) doesn't match schema(%lu fields)", \
                     _n_, schema_.field_num());                         \
            return NULL;                                                \
        }                                                               \
        SCOPED_DYN_TUPLE(tup, &schema_);                                \
        ST_APPL##_n_(DYN_TABLE_INSERT_SET_N);                           \
        return insert_tuple(tup);                                       \
    }

#define DYN_TABLE_KEY_SET_N(_n_)                \
    key.at_n(sm_.pos[_n_]) = (t##_n_)

#define HAVE_DYN_TABLE_ERASER_DO(_n_)                                   \
    template <ST_SYMBOLL##_n_(typename _T)>                             \
    size_t operator()(ST_PARAML##_n_(const _T, & t)) const              \
    {                                                                   \
        if (NULL == sm_.key_schema) {                                   \
            ST_WARN("This eraser is empty");                            \
            return 0;                                                   \
        }                                                               \
        if (_n_ != sm_.key_schema->field_num()) {                       \
            ST_FATAL("Parameters(%d) does not match erased key(%lu fields)", \
                     _n_, sm_.key_schema->field_num());                 \
            return 0;                                                   \
        }                                                               \
        SCOPED_DYN_TUPLE(key, sm_.key_schema);                          \
        ST_APPL##_n_(DYN_TABLE_KEY_SET_N);                              \
        return erase_key(key);                                          \
    }

#define HAVE_DYN_TABLE_SEEKER_DO(_n_)                                   \
    template <ST_SYMBOLL##_n_(typename _T)>                             \
    DynNormalIterator operator()(ST_PARAML##_n_(const _T, & t)) const   \
    {                                                                   \
        if (NULL == sm_.key_schema) {                                   \
            ST_WARN("This seeker is empty");                            \
            return DynNormalIterator(schema_, NULL);                    \
        }                                                               \
        if (_n_ != sm_.key_schema->field_num()) {                       \
            ST_FATAL("Parameters(%d) does not match seeked key(%lu fields)", \
                     _n_, sm_.key_schema->field_num());                 \
            return DynNormalIterator(schema_, NULL);                    \
        }                                                               \
        SCOPED_DYN_TUPLE(key, sm_.key_schema);                          \
        ST_APPL##_n_(DYN_TABLE_KEY_SET_N);                              \
        return DynNormalIterator(schema_, seek_key(key));               \
    }

#define HAVE_DYN_TABLE_SEEKER_DO_MEM(_n_)                               \
    template <ST_SYMBOLL##_n_(typename _T)>                             \
    DynNormalIterator foo(void* mem, ST_PARAML##_n_(const _T, & t)) const \
    {                                                                   \
        if (NULL == sm_.key_schema) {                                   \
            ST_WARN("This seeker is empty");                            \
            return DynNormalIterator(schema_, NULL);                    \
        }                                                               \
        if (_n_ != sm_.key_schema->field_num()) {                       \
            ST_FATAL("Parameters(%d) does not match seeked key(%lu fields)", \
                     _n_, sm_.key_schema->field_num());                 \
            return DynNormalIterator(schema_, NULL);                    \
        }                                                               \
        SCOPED_DYN_TUPLE(key, sm_.key_schema);                          \
        ST_APPL##_n_(DYN_TABLE_KEY_SET_N);                              \
        return DynNormalIterator(schema_, seek_key(key, mem));          \
    }


#define SCOPED_CALL_SEEKER(_name_, _sk_, ...)                           \
    char _name_##_buf[(_sk_).mem_size()];                               \
    DynNormalIterator _name_ = (_sk_).foo(_name_##_buf, __VA_ARGS__);   \

// A table supporting runtime schema
class DynTable {
public:
    static const unsigned int MAX_NCOLUMN = 256u;
    
    struct IndexInfo;
    struct SeekMethod;
    class Eraser;
    class Seeker;

    DynTable();
    ~DynTable();
    DynTable(const DynTable&);
    DynTable& operator=(const DynTable&);

    // Initialize this table, must be called before using
    // The parameter MUST be formatted as: "type1 column1, type2 column2, ..."
    // The types MUST be recognizable by DynTuple
    // Example:
    //   DynTable t;
    //   t.init("uint32 site_id, uint32 unit_id, int32 bid");
    // Returns:
    //   0              success
    //   EINVAL         format of `decl' is invalid
    //   ERANGE         #fields exceeds DynTable::MAX_NCOLUMN
    int init(const char* decl);

    // Add an index
    // Note: A table requires at least one index.
    // Param `index_decl' could be one of the follows:
    //   - ST_UNIQUE_KEY(column1, column2 ... NBUCKET=1000, LOAD_FACTOR=70 ...)
    //     `NBUCKET' and `LOADFACTOR' are optional, if unset, default values are used.
    //   - ST_UNIQUE_KEY(column1, column2 ... ST_CLUSTER_KEY(column3 ... MAX_FANOUT=32))
    //     `MAX_FANOUT' is optional, if unset, default value is used.
    // Returns:
    //   0               success
    //   ECONFLICT       the index breaks unique constraints
    //   ENOMEM          no enough memory to create the index
    //   EINVAL          can't understand `index_decl'
    //   otherwise       what DynUniqueIndex::init(4) returns
    int add_index(const char* index_decl);

    // Format the input and treat as `index_decl' in add_index(1)
    // Returns: what add_index(1) returns
    int add_index_format(const char* fmt, ...);
    
    // Insert into this table.
    // Note:
    //   Number of parameters should exactly match number of the fields,
    //   thus following functions are exclusive for an initialized table.
    // Returns:
    //   address of inserted tuple, NULL means failure
    // Example:
    //   DynTable t;
    //   t.init("uint32 site_id, uint32 unit_id, int32 bid");
    //   t.add_index("ST_UNIQUE_KEY(site_id, unit_id)");
    //   t.insert(1u, 1u, 100);      /* #parameters should be 3 */
    //   t.insert(1u, 1u, 200);      /* overwrite previous entry due to the index */
    //   t.insert(1u, 2u, 120);
    HAVE_DYN_TABLE_INSERT(1);
    HAVE_DYN_TABLE_INSERT(2);
    HAVE_DYN_TABLE_INSERT(3);
    HAVE_DYN_TABLE_INSERT(4);
    HAVE_DYN_TABLE_INSERT(5);
    HAVE_DYN_TABLE_INSERT(6);
    HAVE_DYN_TABLE_INSERT(7);
    HAVE_DYN_TABLE_INSERT(8);
    HAVE_DYN_TABLE_INSERT(9);

    // Insert `values' separated by `sep' into this table
    // Returns:
    //   address of inserted tuple, NULL means failure
    void* insert_by_string(const char* values, char sep = ',');
    
    // Insert a DynTuple into this table
    // Note:
    //   schema of the DynTuple MUST be same with this table(not checked yet)
    // Returns:
    //   address of inserted tuple, NULL means failure
    void* insert_tuple(const DynTuple&);

    // Make an eraser on `key_names' separated by `sep'. Erasing from a DynTable
    // is designed as making a eraser first and do actual erasures by
    // DynTable::Eraser::operator().
    // Note:
    //   The `key_names' MUST be indexed by an index.
    //   Order of keys are not important.
    // Example:
    //   DynTable::Eraser es = t.make_eraser("site_id, unit_id");
    //   es(11, 30);
    //   es(10, 20);
    Eraser make_eraser(const char* key_names, char sep = ',') const;

    // Make an seeker on `key_names' separated by `sep', Seeking a DynTable
    // is designed as making a seeker first and do actual erasures by
    // DynTable::Seeker::operator().
    // Note:
    //   The `key_names' MUST be indexed by an index
    //   order of keys are not required.
    // Example:
    //   DynTable::Seeker sk = t.make_seeker("site_id, unit_id");
    //   sk(11, 30);
    //   sk(10, 20);
    Seeker make_seeker(const char* key_names, char sep = ',') const;
    
    DynNormalIterator all() const;   // Get an` iterator to iterate all elements
    void clear();                    // Remove all elements
    void reserve(size_t);            // Reserve space for internal indexes
    size_t size() const;             // Get #elements
    bool empty() const;              // Test if empty
    bool not_init() const;           // Test if not initialized
    size_t mem() const;              // Get approximate resident memory
    
    const DynTupleSchema* schema() const { return &schema_; }
    size_t index_num() const { return index_list_.size(); }
    DynIndex* index_at(size_t idx) const;

friend std::ostream& operator<<(std::ostream&, const DynTable&);
private:
    typedef std::vector<IndexInfo> IndexInfoList;

    SeekMethod find_exact_index(const char* key_names, char sep) const;

    DynIndex* check_uniqueness(DynIndex* index) const;

    // `*rc' is one of: 0, EINVAL, ENOMEM
    DynIndex* try_as_unique_index(int* rc, const char* index_decl) const;

    // Returns: 0, ECONFLICT
    int add_index_and_update_info(DynIndex*, const char*);

    void insert_into_index_info_list(const IndexInfo&);
    
    DynIndex* first_index_;
    std::vector<DynIndex*> index_list_;
    IndexInfoList index_info_list_;
    DynTupleSchema schema_;
    int traverse_index_pos_;
};

struct DynTable::IndexInfo {
    int index_pos;
    int seek_info_pos;
    DynSeekInfo si;
};
    
struct DynTable::SeekMethod {
    SeekMethod()
        : key_schema(NULL), pos(NULL), index(NULL), seek_info_pos(-1) {}
    SeekMethod(const DynTupleSchema* ks, const size_t* p, DynIndex* i, int sip)
        : key_schema(ks), pos(p), index(i), seek_info_pos(sip) {}
        
    const DynTupleSchema* key_schema;
    const size_t* pos;
    DynIndex* index;    // the index to be seeked
    int seek_info_pos;
};        

class DynTable::Eraser {
public:        
    // Erase from the host table, #parameters must match #key_names in
    // host_table.make_eraser(key_names) that makes this eraser
    // Returns:
    //   Number of erased elements
    HAVE_DYN_TABLE_ERASER_DO(1);
    HAVE_DYN_TABLE_ERASER_DO(2);
    HAVE_DYN_TABLE_ERASER_DO(3);
    HAVE_DYN_TABLE_ERASER_DO(4);
    HAVE_DYN_TABLE_ERASER_DO(5);

    // Erase `key_values' separated by `sep' from the host table
    size_t erase_by_string(const char* key_values, char sep = ',');

    // Test if this eraser is valid, NULL means invalid
    // Note: Don't store and make use of the pointer!
    operator const void*() const { return sm_.key_schema; }
        
private:
friend class DynTable;
    Eraser(const SeekMethod& sm, const DynTable* t) : sm_(sm), table_(t) {}
    size_t erase_key(const DynTuple&) const;
    SeekMethod sm_;                        
    const DynTable* table_;          // host table
};

class DynTable::Seeker {
public:
    // Seek the host table, #parameters must match #key_names in
    // host_table.make_seeker(key_names) that makes this seekr
    // Returns:
    //   An iterator to iterate matched tuples
    HAVE_DYN_TABLE_SEEKER_DO(1);
    HAVE_DYN_TABLE_SEEKER_DO(2);
    HAVE_DYN_TABLE_SEEKER_DO(3);
    HAVE_DYN_TABLE_SEEKER_DO(4);
    HAVE_DYN_TABLE_SEEKER_DO(5);

    HAVE_DYN_TABLE_SEEKER_DO_MEM(1);
    HAVE_DYN_TABLE_SEEKER_DO_MEM(2);
    HAVE_DYN_TABLE_SEEKER_DO_MEM(3);
    HAVE_DYN_TABLE_SEEKER_DO_MEM(4);
    HAVE_DYN_TABLE_SEEKER_DO_MEM(5);
    
    // Seek `key_values' separated by `sep' from the host table
    DynNormalIterator seek_by_string(const char* key_values, char sep=',');

    // Test if this seeker is valid, NULL means invalid
    // Note: Don't store and make use of the pointer!
    operator const void*() const { return sm_.key_schema; }

    size_t mem_size() const { return sm_.index ? sm_.index->seek_mem_size() : 0; }
    
friend std::ostream& operator<<(std::ostream&, const DynTable::Seeker&);
    
private:
friend class DynTable;
    Seeker(const SeekMethod& sm, const DynTupleSchema* s) : sm_(sm), schema_(s) {}

    DynIterator* seek_key(const DynTuple& key) const
    { return sm_.index->seek(sm_.seek_info_pos, key); }

    DynIterator* seek_key(const DynTuple& key, void* mem) const
    { return sm_.index->seek(sm_.seek_info_pos, key, mem); }

    SeekMethod sm_;
    const DynTupleSchema* schema_;
};


}  // namespace st

#endif  //_DYN_TABLE_H_
