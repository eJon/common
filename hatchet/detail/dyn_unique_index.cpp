// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Implement dyn_unique_index.h
// Author: gejun@baidu.com
// Date: Dec.27 15:28:29 CST 2010
#include "dyn_unique_index.h"
#include <sstream>                   //  ostringstream

namespace st {

DynUniqueIndex::DynUniqueIndex() : DynIndex()
{}

DynUniqueIndex::~DynUniqueIndex ()
{}

DynUniqueIndex::DynUniqueIndex(const DynUniqueIndex& rhs) : DynIndex(rhs)
{
    if (copy_from(rhs)) {
        ST_FATAL("Fail to copy-construct from `rhs'");
    }
}

DynUniqueIndex& DynUniqueIndex::operator=(const DynUniqueIndex& rhs)
{
    if (copy_from(rhs)) {
        ST_FATAL("Fail to assign from `rhs'");
    }
    return *this;
}

int DynUniqueIndex::try_copy_from(const DynIndex& base_rhs) const
{
    return dynamic_cast<const DynUniqueIndex*>(&base_rhs) ? 0 : EINVAL;
}

int DynUniqueIndex::copy_from(const DynIndex& base_rhs)
{
    const DynUniqueIndex* rhs = dynamic_cast<const DynUniqueIndex*>(&base_rhs);
    if (NULL == rhs) {
        return EINVAL;
    }
    base_ = rhs->base_;
    key_pos_ = rhs->key_pos_;
    key_schema_ = rhs->key_schema_;
    ih_.index_ = this;
    schema_ = rhs->schema_;
    
    return 0;
}

DynIndex* DynUniqueIndex::clone() const
{
    return new (std::nothrow) DynUniqueIndex(*this);
}

int DynUniqueIndex::init (
    const DynTupleSchema* schema,
    std::vector<std::string> key_names,
    size_t n_bucket,
    u_int load_factor)
{
    if (NULL == schema) {
        ST_FATAL("Param[schema] is NULL");
        return EINVAL;
    }
    schema_ = schema;

    for (size_t i = 0; i < key_names.size(); ++i) {
        const DynTupleAccess* e = schema->at(key_names[i].c_str());
        if (NULL == e) {
            ST_FATAL("`%s' is not an existing column", key_names[i].c_str());
            return ENOTEXIST;
        }
        key_pos_.push_back(schema->find_name(key_names[i].c_str()));
        key_schema_.add_field(e->name.c_str(), e->type, e->width);
    }
    
    std::ostringstream oss;
    oss << "Item=" << *schema << " Key=" << key_schema_;
    ih_.index_ = this;
    return base_.init(schema->byte_size(),
                      n_bucket,
                      load_factor,
                      &ih_,
                      oss.str());
}

void* DynUniqueIndex::insert(const DynTuple& tup)
{
    char key_buf[key_schema_.byte_size()];
    DynTuple key(&key_schema_, key_buf);
    for (size_t i = 0; i < key_pos_.size(); ++i) {
        key.at_n(i) = tup.at_n(key_pos_[i]);
    }
    return base_.insert(key_buf, tup.buf());
}

std::vector<DynSeekInfo> DynUniqueIndex::seek_info_list() const
{
    std::vector<DynSeekInfo> l;
    DynSeekInfo si;
    si.key_schema = &key_schema_;
    si.seek_score = COW_HASH_SCORE;
    si.partial_seek_score = MAYBE_COW_HASH_SCORE;
    l.push_back(si);
    return l;
}

size_t DynUniqueIndex::erase(int seek_info_pos, const DynTuple& key)
{
    if (seek_info_pos != 0) {
        ST_FATAL("seek_info_pos must be zero in DynUniqueIndex::erase");
        return 0;
    }
    return static_cast<size_t>(base_.erase(key.buf()));
}

DynIterator* DynUniqueIndex::seek(int si_pos, const DynTuple& key) const
{
    if (si_pos) {
        ST_FATAL("Position of seekinfo must be 0 in DynUniqueIndex::seek");
        return NULL;
    }
    void* item = base_.seek(key.buf());
    return item ? new (std::nothrow) DynUniqueIndexSeekIterator(item) : NULL;
}

DynIterator* DynUniqueIndex::seek(int si_pos, const DynTuple& key, void* mem) const
{
    if (si_pos) {
        ST_FATAL("Position of seekinfo must be 0 in DynUniqueIndex::seek");
        return NULL;
    }
    void* item = base_.seek(key.buf());
    return item ? new (mem) DynUniqueIndexSeekIterator(item) : NULL;
}

DynIterator* DynUniqueIndex::maybe_seek(
    int /*seek_info_pos*/, const DynTuple& /*key*/) const
{
    return NULL;
    // void* item = base_.seek(key.buf());
    // if (item) {
    //     p_it->iterator().set_value(item);
    //     p_it->point_to_iterator();
    // } else {
    //     copy_attr_list<_Tuple, Key1, class Key1::AttrS>::
    //         call(&(p_it->value()), key);
    //     p_it->point_to_value();
    // }
}

DynIterator* DynUniqueIndex::exclude_seek(
    int /*seek_info_pos*/, const DynTuple& /*key*/) const
{
    return NULL;
    // Pointer p_item = base_.seek(key.buf());
    // if (p_item) {
    //     p_it->iterator().set_end();
    //     p_it->point_to_iterator();
    // } else {
    //     copy_attr_list<_Tuple, Key1, class Key1::AttrS>::
    //         call(&(p_it->value()), key);
    //     p_it->point_to_value();
    // }
}

size_t DynUniqueIndex::key_num(int seek_info_pos) const
{
    if (seek_info_pos != 0) {
        ST_FATAL("seek_info_pos must be zero in DynUniqueIndex::key_num");
        return 0;
    }
    return base_.size();
}

bool DynUniqueIndex::copy_key_part(DynTuple* p_dst, const DynTuple& src) const
{
    for (size_t i = 0; i < key_pos_.size(); ++i) {
        p_dst->at_n(key_pos_[i]) = src.at_n(key_pos_[i]);
    }
    return true;
}

DynSeekInfo DynUniqueIndex::uniqueness() const
{
    return seek_info_list()[0];
}
    
void* DynUniqueIndex::seek_tuple(const DynTuple& tup) const
{
    SCOPED_DYN_TUPLE(key, &key_schema_);
    for (size_t i = 0; i < key_pos_.size(); ++i) {
        key.at_n(i) = tup.at_n(key_pos_[i]);
    }
    return base_.seek(key.buf());
}    

bool DynUniqueIndex::erase_tuple(const DynTuple& tup)
{
    SCOPED_DYN_TUPLE(key, &key_schema_);
    for (size_t i = 0; i < key_pos_.size(); ++i) {
        key.at_n(i) = tup.at_n(key_pos_[i]);
    }
    return base_.erase(key.buf());
}

DynIterator* DynUniqueIndex::all() const
{
    return new (std::nothrow) DynUniqueIndexIterator(&base_);
}

void DynUniqueIndex::serialize(std::ostream& os) const
{
    os << "DynUniqueIndex{" << base_ << "}";
}

size_t DynUniqueIndex::ItemHandler::hash_key(const void* key) const
{
    return DynTuple(&index_->key_schema_, (void*)key).hash_code();
}

void DynUniqueIndex::ItemHandler::get_key(void* buf, const void* item) const
{
    DynTuple key(&index_->key_schema_, buf);
    DynTuple tup(index_->schema_, (void*)item);
    std::vector<int> const& key_pos = index_->key_pos_;
    for (size_t i = 0; i < key_pos.size(); ++i) {
        key.at_n(i) = tup.at_n(key_pos[i]);
    }
}

bool DynUniqueIndex::ItemHandler::eq_key_item(
    const void* buf, const void* item) const
{
    DynTuple key(&index_->key_schema_, (void*)buf);
    DynTuple tup(index_->schema_, (void*)item);
    std::vector<int> const& key_pos = index_->key_pos_;
    for (size_t i = 0; i < key_pos.size(); ++i) {
        if (key.at_n(i) != tup.at_n(key_pos[i])) {
            return false;
        }
    }
    return true;
}

void DynUniqueIndex::ItemHandler::copy_item(
    void* dst_item, const void* src_item) const
{
    memcpy(dst_item, src_item, index_->schema_->byte_size());
}

void DynUniqueIndex::ItemHandler::copy_construct_item(
    void* dst_item, const void* src_item) const
{
    memcpy(dst_item, src_item, index_->schema_->byte_size());
}

void DynUniqueIndex::ItemHandler::destruct_item(void*) const
{
    /* do nothing */
}

}  // namespace st
