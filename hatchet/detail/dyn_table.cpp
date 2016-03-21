// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Implement dyn_table.h
// Author: gejun@baidu.com
// Date: Jun 22 17:21:40 CST 2011

#include <ctype.h>                   // isblank
#include "dyn_table.h"
#include "dyn_unique_index.h"        // DynUniqueIndex
#include "string_reader.hpp"         // StringReader

namespace st {

template <typename _T>
std::ostream& operator<<(std::ostream& os, std::vector<_T> const& v)
{
    bool not_first = false;
    os << '[';
    for (class std::vector<_T>::const_iterator it = v.begin();
         it != v.end(); ++it) {
        if (not_first) {
            os << ", ";
        } else {
            not_first = true;
        }
        os << *it;
    }
    os << ']';
    return os;
}

static bool set_equal(std::vector<std::string> const& key_set1,
                      const DynTupleSchema& key_schema,
                      size_t* pos)
{
    if (key_set1.size() != key_schema.field_num()) {
        return false;
    }
    
    for (size_t i = 0; i < key_set1.size(); ++i) {
        bool matched = false;
        for (size_t j = 0; j < key_schema.field_num(); ++j) {
            if (0 == strcmp(key_schema.at_n(j)->name.c_str(),
                            key_set1[i].c_str())) {
                pos[i] = j;
                matched = true;
                break;
            }
        }
        if (!matched) {
            return false;
        }
    }
    return true;
}

static bool set_greater_equal(const DynTupleSchema& ks1,
                              const DynTupleSchema& ks2)
{
    for (size_t j = 0; j < ks2.field_num(); ++j) {
        bool matched = false;
        for (size_t i = 0; i < ks1.field_num(); ++i) {
            if (0 == strcmp(ks2.at_n(j)->name.c_str(), ks1.at_n(i)->name.c_str())) {
                matched = true;
                break;
            }
        }
        if (!matched) {
            return false;
        }
    }
    
    return true;
}


DynTable::DynTable()
    : first_index_(NULL)
    , traverse_index_pos_(-1)
{   
}

DynTable::~DynTable()
{
    first_index_ = NULL;
    for (size_t i = 0; i < index_list_.size(); ++i) {
        delete index_list_[i];
    }
    index_list_.clear();
    schema_.clear();
    traverse_index_pos_ = -1;
}

DynTable::DynTable(const DynTable& rhs)
{
    if (rhs.not_init()) {
        ST_FATAL("Copy-construct from an uninitialized DynTable, do nothing");
        return;
    }

    if (!not_init()) {
        ST_FATAL("This DynTable is already initialized, force destruction");
        this->~DynTable();
    }

    schema_ = rhs.schema_;
    traverse_index_pos_ = rhs.traverse_index_pos_;
    
    first_index_ = NULL;
    index_list_.clear();
    index_info_list_.clear();
    for (size_t i = 0; i < rhs.index_list_.size(); ++i) {
        DynIndex* clone = rhs.index_list_[i]->clone();
        if (NULL == clone) {
            ST_FATAL("Fail to clone index %zu of rhs", i);
            return;
        }
        clone->set_schema(&schema_);
        if (add_index_and_update_info(clone, "")) {
            ST_FATAL("Fail to add cloned index %zu", i);
            return;
        }
    }
}

DynTable& DynTable::operator=(const DynTable& rhs)
{
    if (not_init()) {
        new (this) DynTable(rhs);
        return *this;
    }

    if (rhs.not_init()) {
        ST_FATAL("Copy-construct from an uninitialized DynTable, force destruction");
        this->~DynTable();
    }    
    
    if (index_list_.size() != rhs.index_list_.size()) {
        ST_FATAL("Number of indexes(%lu) do not match rhs(%lu), quit",
                 index_list_.size(), rhs.index_list_.size());
        return *this;
    }

    if (schema_ != rhs.schema_) {
        ST_FATAL("Schema does not match, quit");
        return *this;
    }
    
    for (size_t i = 0; i < index_list_.size(); ++i) {
        if (index_list_[i]->try_copy_from(*rhs.index_list_[i])) {
            ST_FATAL("Fail to copy from index %zu from `rhs', quit", i);
            return *this;
        }
    }

    for (size_t i = 0; i < index_list_.size(); ++i) {
        if (index_list_[i]->copy_from(*rhs.index_list_[i])) {
            ST_FATAL("Fail to copy from index %zu from `rhs', Impossible!", i);
            return *this;
        }
    }

    return *this;
}

int DynTable::init(const char* header)
{
    int rc;
    
    if ((rc = schema_.add_fields_by_string(header, MAX_NCOLUMN))) {
        ST_FATAL("Fail to create schema `%s'", header);
        return rc;
    }
    return 0;
}

DynIndex* DynTable::check_uniqueness(DynIndex* index) const
{
    if (NULL == first_index_) {
        return index;
    }

    const DynTupleSchema* ks1 = first_index_->uniqueness().key_schema;
    const DynTupleSchema* ks2 = index->uniqueness().key_schema;
    
    if (set_greater_equal(*ks1, *ks2)) {
        return index;
    } else if (set_greater_equal(*ks2, *ks1)) {
        return first_index_;
    }
    return NULL;
}

DynIndex* DynTable::try_as_unique_index(int* rc_out, const char* index_decl) const
{
    // " ST_UNIQUE_KEY ( a , b , c, NBUCKET = 1000 , LOADFACTOR = 60) "
    StringReader sr(index_decl);
    size_t nbucket = CH_DEFAULT_NBUCKET;
    unsigned int load_factor = CH_DEFAULT_LOAD_FACTOR;
    std::string key_name_buf;
    std::vector<std::string> key_names;
    int rc;

    HAVE_MANIPULATOR(
        read_arg1,
        ((once("NBUCKET") >> many0(' ') >> '=' >> many0(' ') >> &nbucket) |
         (once("LOADFACTOR") >> many0(' ') >> '=' >> many0(' ') >> &load_factor) |
         (read_name1(&key_name_buf) >> sr_push_back(key_name_buf, key_names))));
    sr >> many0(' ') >> "ST_UNIQUE_KEY" >> many0(' ') >> '('
       >> many0(' ') >> read_arg1
       >> many0(many0(' ') >> ',' >> many0(' ') >> read_arg1)
       >> many0(' ') >> ')' >> many0(' ') >> sr_ends;
        

    if (!sr.good()) {
        *rc_out = EINVAL;
        ST_FATAL("Fail to parse `%s', stopped at `%s'", index_decl, sr.buf());
        return NULL;
    }
    
    DynUniqueIndex* index1 = new (std::nothrow) DynUniqueIndex;
    if (NULL == index1) {
        *rc_out = ENOMEM;
        ST_FATAL("Fail to new DynUniqueIndex");
        return NULL;
    }

    if ((rc = index1->init(&schema_, key_names, nbucket, load_factor))) {
        *rc_out = rc;
        ST_FATAL("Fail to init DynUniqueIndex by `%s'", index_decl);
        return NULL;
    }

    *rc_out = 0;
    return index1;
}

int DynTable::add_index(const char* index_decl)
{
    int rc = 0;
    DynIndex* index1 = NULL;
    
    index1 = try_as_unique_index(&rc, index_decl);
    if (index1) {
        return add_index_and_update_info(index1, index_decl);
    } else if (EINVAL != rc) {
        return rc;
    }

    // try other indexes

    ST_FATAL("Fail to understand `%s'", index_decl);
    return EINVAL;
}

int DynTable::add_index_format(const char* fmt, ...)
{
    size_t len = (size_t)(strlen(fmt) * 1.5);
    int n = 0;
    va_list ap;

    while (1) {
        char buf[len];
        va_start(ap, fmt);
        n = vsnprintf(buf, len, fmt, ap);
        va_end(ap);

        if (n > -1 && n < (int)len) {
            return add_index(buf);
        }
        // Else try again with more space.
        if (n > -1) {
            len = n + 1;  // precisely what is needed
        } else {
            len *= 2;   // twice the old size
        }
    }
}

int DynTable::add_index_and_update_info(DynIndex* index, const char* index_decl)
{
    DynIndex* first_index = check_uniqueness(index);
    if (NULL == first_index) {
        ST_FATAL("Index `%s' breaks unique constraints", index_decl);
        return ECONFLICT;
    }

    first_index_ = first_index;
    index_list_.push_back(index);

    std::vector<DynSeekInfo> si_list = index->seek_info_list();
    for (size_t i = 0; i < si_list.size(); ++i) {
        IndexInfo ii = { (int)index_list_.size() - 1, 
                         (int)i, si_list[i] };
        insert_into_index_info_list(ii);
    }
    return 0;
}

void DynTable::insert_into_index_info_list(const IndexInfo& ii)
{
    IndexInfoList::iterator it = index_info_list_.begin();
    for ( ; it != index_info_list_.end() &&
              !set_greater_equal(*ii.si.key_schema, *it->si.key_schema); ++it);
    index_info_list_.insert(it, ii);
}

void* DynTable::insert_by_string(const char* values, char sep)
{
    SCOPED_DYN_TUPLE(tup, &schema_);
    if (0 == tup.set_by_string(values, sep)) {
        return insert_tuple(tup);
    }
    ST_FATAL("The values(%s) does not match the schema", values);
    return NULL;
}

void* DynTable::insert_tuple(const DynTuple& tup)
{
    if (1ul == index_list_.size()) {     // short path
        return index_list_[0]->insert(tup);
    }
    
    void* replaced_buf = first_index_->seek_tuple(tup);
    if (replaced_buf) {
        DynTuple replaced_tup(&schema_, replaced_buf);
        for (std::vector<DynIndex*>::iterator it = index_list_.begin();
             it != index_list_.end(); ++it) {
            if (*it != first_index_) {
                (*it)->erase_tuple(replaced_tup);
            }
        }
    }
    for (std::vector<DynIndex*>::iterator it = index_list_.begin();
         it != index_list_.end(); ++it) {
        if (*it != first_index_) {
            (*it)->insert(tup);
        }
    }
    return first_index_->insert(tup);
}

std::ostream& operator<<(std::ostream& os, const DynTable::SeekMethod& sm)
{
    os << "SeekMethod{Schema=";
    if (sm.key_schema) {
        os << *sm.key_schema << " pos=[";
        for (size_t i = 0; i < sm.key_schema->field_num(); ++i) {
            os << sm.pos[i] << ' ';
        }
        os << ']';
    } else {
        os << "Nil";
    }

    if (sm.index) {
        os << " Index=" << *sm.index;
    }
    os << " SeekInfoPos=" << sm.seek_info_pos << '}';
    return os;
}

std::ostream& operator<<(std::ostream& os, const DynTable::Seeker& s)
{
    return os << s.sm_;
}


DynTable::SeekMethod DynTable::find_exact_index(
    const char* keys_str, char sep) const
{
    IndexInfoList::const_iterator it;
    std::string key_name_buf;
    std::vector<std::string> key_names;

    // " a , b , c "
    StringReader sr(keys_str);
    HAVE_MANIPULATOR(
        read_key_name,
        (read_name1(&key_name_buf) >> sr_push_back(key_name_buf, key_names)));
    if (' ' != sep) {
        sr >> many0(' ') >> read_key_name >> many0(' ')
           >> many0(once(sep) >> many0(' ') >> read_key_name >> many0(' '))
           >> sr_ends;
    } else {
        sr >> many0(' ') >> read_key_name
           >> many0(many1(' ') >> read_key_name)
           >> many0(' ') >> sr_ends;
    }

    if (!sr.good()) {
        ST_FATAL("Invalid format `%s', stopped at `%s'", keys_str, sr.buf());
        return SeekMethod();
    }

    size_t* pos = new (std::nothrow) size_t[key_names.size()];
    for (it = index_info_list_.begin(); it != index_info_list_.end()
             && !set_equal(key_names, *it->si.key_schema, pos); ++it);
    
    if (it == index_info_list_.end()) {
        ST_FATAL("Can't find an index on `%s'", keys_str);
        return SeekMethod();
    }

    return SeekMethod(it->si.key_schema,
                      pos,
                      index_list_[it->index_pos],
                      it->seek_info_pos);
}

DynTable::Eraser DynTable::make_eraser(const char* key_names, char sep) const
{
    return Eraser(find_exact_index(key_names, sep), this);
}

size_t DynTable::Eraser::erase_key(const DynTuple& key) const
{
    if (1ul == table_->index_num()) {
        return sm_.index->erase(sm_.seek_info_pos, key);
    }

    DynIterator* it = sm_.index->seek(sm_.seek_info_pos, key);
    if (NULL == it) {
        return 0;
    }

    for (const void* p = it->setup(); p; p = it->forward()) {
        DynTuple erased_tup(&table_->schema_, (void*)p);
        for (std::vector<DynIndex*>::const_iterator it2 = table_->index_list_.begin();
             it2 != table_->index_list_.end(); ++it2) {
            if ((*it2) != sm_.index) {
                if (!(*it2)->erase_tuple(erased_tup)) {
                    ST_FATAL("Fail to erase from this index");
                }
            }
        }
    }
    return sm_.index->erase(sm_.seek_info_pos, key);
}

size_t DynTable::Eraser::erase_by_string(const char* values, char sep)
{
    if (NULL == *this) {
        ST_WARN("This eraser is empty");
        return 0;
    }
    SCOPED_DYN_TUPLE(key, sm_.key_schema);
    if (key.set_by_string(values, sep, sm_.pos) != 0) {
        ST_FATAL("The values(%s) does not match the key", values);
        return 0;
    }
    return erase_key(key);
}

DynTable::Seeker DynTable::make_seeker(const char* key_names, char sep) const
{
    return Seeker(find_exact_index(key_names, sep), &schema_);
}

DynNormalIterator DynTable::Seeker::seek_by_string(const char* values, char sep)
{
    if (NULL == *this) {
        ST_WARN("This seeker is empty");
        return DynNormalIterator(schema_, NULL); 
    }
    SCOPED_DYN_TUPLE(tup, sm_.key_schema);
    if (tup.set_by_string(values, sep, sm_.pos) != 0) {
        ST_FATAL("The values(%s) does not match the schema", values);
        return DynNormalIterator(schema_, NULL); 
    }
    return DynNormalIterator(schema_, seek_key(tup));
}

void DynTable::clear()
{
    for (size_t i = 0; i < index_list_.size(); ++i) {
        index_list_[i]->clear();
    }
}
    
void DynTable::reserve(size_t size)
{
    for (size_t i = 0; i < index_list_.size(); ++i) {
        index_list_[i]->resize(size);
    }
}

size_t DynTable::size() const
{
    return index_list_.empty() ? 0 : index_list_[0]->size();
}

bool DynTable::empty() const
{
    return index_list_.empty() || index_list_[0]->empty();
}

bool DynTable::not_init() const
{
    return schema_.empty() || index_list_.empty();
}

size_t DynTable::mem() const
{
    size_t rc = 0;
    for (size_t i = 0; i < index_list_.size(); ++i) {
        rc += index_list_[i]->mem();
    }
    return rc;
}

DynNormalIterator DynTable::all() const
{
    return DynNormalIterator(&schema_, first_index_ ? first_index_->all() : NULL);
}

DynIndex* DynTable::index_at(size_t i) const
{
    return i < index_list_.size() ? index_list_[i] : NULL;
}

std::ostream& operator<<(std::ostream& os, const DynTable::IndexInfo& ii)
{
    return os << "IndexInfo(" << ii.index_pos << "/"
              << ii.seek_info_pos << "):" << ii.si;
}

std::ostream& operator<<(std::ostream& os, const DynTable& t)
{
    int fi_pos = (int)t.index_list_.size() - 1;
    for ( ; fi_pos >= 0 &&
              t.first_index_ != t.index_list_[fi_pos]; --fi_pos);
    
    return os << "DynTable{Schema=" << t.schema_
              << " Indexes(first=" << fi_pos
              << ",traverse=" << t.traverse_index_pos_
              << ")=" << t.index_list_.size() << ':' << t.index_list_
              << " IndexInfoL=" << t.index_info_list_
              << '}';
}

}  // namespace st
