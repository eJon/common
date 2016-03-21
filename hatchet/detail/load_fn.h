// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Be used to load CowTable.
// Author: wangqiushi@baidu.com
// Date: Jul 6 12:00:00 CST 2012
#pragma once
#ifndef _LOAD_FN_HPP_
#define _LOAD_FN_HPP_

#include "debug.h"
#include "binary_file_reader.h"

namespace st {
    
class LoadFn {
public:
    enum LoadState {
        SUCCEED,
        FAIL,
        END
    };
    
    LoadFn(size_t item_size)
        : _item_size(item_size),
        _cur_item(NULL),
        _cur_field(NULL),
        _reader(item_size) { }
    ~LoadFn() { close(); }
    
    template <class META_READER>
    int init(const char* dir, const char* filename, META_READER* meta_reader);

    int close();

    template <typename T>
    int operator()(const int &state, T &value) const;
    
    LoadState read_next(int steps = 1) {
        if (0 >= steps) {
            return SUCCEED;
        }

        LoadState state = FAIL;
        for (int i = 0; i < steps; i++) {
            switch(_reader.read_next()) {
                case BinaryFileReader::SUCCEED:
                    _cur_item = _reader.get_value();
                    _cur_field = _cur_item;
                    state = SUCCEED;
                    break;
                case BinaryFileReader::END:
                    _cur_item = NULL;
                    _cur_field = NULL;
                    state = END;
                    break;
                case BinaryFileReader::FAIL:
                    _cur_item = NULL;
                    _cur_field = NULL;
                    state = FAIL;
                    break;
                default:
                    state = FAIL;
                    break;
            }

            if (SUCCEED != state) {
                break;
            }
        }

        return state;
    }
    
    bool is_end() { return NULL == _cur_item; }

    int reset() {
        return _reader.reset();
    }
private:
    const size_t _item_size;
    std::string _filename;
    mutable const char *_cur_item;
    mutable const char *_cur_field;
    mutable BinaryFileReader _reader;
};

template <class META_READER>
int LoadFn::init(const char* dir, const char* filename, META_READER* meta_reader) {
    _filename.clear();
    _filename.append(dir).append("/").append(filename);

    if (_reader.init() != 0) {
        ST_FATAL("fail to init reader, file [%s].", _filename.c_str());
        return -1;
    }

    if (_reader.open(dir, filename, meta_reader) != 0) {
        ST_FATAL("fail to open reader, file [%s].", _filename.c_str());
        return -1;
    }

    _cur_item = _reader.get_value();
    _cur_field = _cur_item;

    return 0;
}

template <typename T>
int LoadFn::operator()(const int &state, T &value) const {
    if (0 != state) {
        return state;
    }
    
    if (NULL == _cur_item || NULL == _cur_field) {
        ST_FATAL("binary reader is not open or has no data, file [%s].", _filename.c_str());
        return -1;
    }
    if (_cur_field < _cur_item
        || _cur_field >= _cur_item + _item_size) {
        ST_FATAL("field pointer is incorrect, internal error in LoadFn, file [%s].", _filename.c_str());
        return -1;
    }   
    
    const T *ptr = reinterpret_cast<const T*>(_cur_field);
    value = *ptr;

    _cur_field += sizeof(T);

    return 0;
}

} // namespace st

#endif // _LOAD_FN_HPP_
