// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Binary file reader.
// Author: wangqiushi@baidu.com
// Date: Jul 6 12:00:00 CST 2012
#pragma once
#ifndef _BINARY_FILE_READER_H_
#define _BINARY_FILE_READER_H_

#include "meta_reader.hpp"

namespace st {
    
static const int DEFAULT_READ_ITEM_COUNT = 1024;

class BinaryFileReader {
public:
    enum ReadState {
        SUCCEED,
        FAIL,
        END
    };
    
    explicit BinaryFileReader(size_t item_size)
        : _buffer(NULL),
        _buffer_len(0),
        _item_size(item_size),
        _item_count(0),
        _cur_item(NULL),
        _filename(""),
        _data_beginning(-1),
        _fp(NULL) {}
    ~BinaryFileReader() { close(); }

    int init(int read_item_once = DEFAULT_READ_ITEM_COUNT);

    template <class META_READER>
    int open(const char* dir, const char* filename, META_READER* meta_reader);
    
    int close();
    
    ReadState read_next();

    const char* get_value() const { return _cur_item; }

    int reset() {
        int ret = fseek(_fp, _data_beginning, SEEK_SET);
        if (0 != ret) {
            ST_FATAL("fail to seek to position [%ld] of file [%s], error [%m].",
                    _data_beginning, _filename.c_str());
            return -1;
        }
        
        if (FAIL == reset_buffer()) {
            ST_FATAL("fail to read data from file [%s].", _filename.c_str());
            return -1;
        }
        return 0;
    }

private:
    ReadState reset_buffer();
    
    char *_buffer;
    size_t _buffer_len;
    const size_t _item_size;
    size_t _item_count;
    const char *_cur_item;
    std::string _filename;
    long _data_beginning;

    FILE *_fp;
};

template <class META_READER>
int BinaryFileReader::open(const char* dir, const char* filename, META_READER* meta_reader) {
    _filename.clear();
    _filename.append(dir).append("/").append(filename);
    
    _fp = fopen(_filename.c_str(), "rb");
    if (NULL == _fp) {
        ST_FATAL("fail to open file [%s].", _filename.c_str());
        return -1;
    }
    
    int ret = 0;
    
    // read meta data in the beginning of file.
    if (NULL != meta_reader) {
        ret = meta_reader->read(&_fp);
        if (0 != ret) {
            ST_FATAL("fail to read meta data from file [%s].", _filename.c_str());
            return -1;
        }
    }
    
    _data_beginning = ftell(_fp);

    if (FAIL == reset_buffer()) {
        ST_FATAL("fail to read data from file [%s].", _filename.c_str());
        return -1;
    }

    return 0;
}

} // namespace st

#endif // _BINARY_FILE_READER_H_
