// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Be used to dump CowTable.
// Author: wangqiushi@baidu.com
// Date: Jul 6 12:00:00 CST 2012
#pragma once
#ifndef _DUMP_FN_HPP_
#define _DUMP_FN_HPP_

#include "debug.h"
#include "meta_reader.hpp"

namespace st {

class DumpFn {
public:
    DumpFn(size_t item_size)
        : _item_size(item_size),
        _buffer(NULL),
        _cur_pos(0),
        _fp(NULL) { }
    ~DumpFn() { close(); }
    
    int init(const char* dir, const char* filename);

    // clear buffer.
    int reset();
    
    // write meta data into file.
    int write_meta_data(const MetaData& meta_data);
    
    // write data in buffer into file.
    int write();
    
    int close();
     
    template <typename T>
    int operator()(const int &state, T &value) const;
private:
    const size_t _item_size;
    mutable char* _buffer;
    mutable size_t _cur_pos;

    std::string _filename;
    
    mutable FILE* _fp;
};

template <typename T>
int DumpFn::operator()(const int &state, T &value) const {
    if (0 != state) {
        return state;
    }
    
    if (_cur_pos >= _item_size) {
        ST_FATAL("buffer overflow.");
        return -1;
    }
        
    if (NULL == memcpy(_buffer + _cur_pos, &value, sizeof(value))) {
        ST_FATAL("fail to write value into buffer of file [%s].", _filename.c_str());
        return -1;
    }
    
    _cur_pos += sizeof(value);

    return 0;
}

} // namespace st

#endif // _DUMP_FN_HPP_
