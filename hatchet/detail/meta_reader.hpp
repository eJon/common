// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Meta data reader.
// Author: wangqiushi@baidu.com
// Date: Jul 6 12:00:00 CST 2012
#pragma once
#ifndef _META_READER_HPP_
#define _META_READER_HPP_

#include "debug.h"

namespace st {

template <class META_DATA>
class CommonMetaReader {
public:
    typedef META_DATA MetaData;
    
    int read(FILE** fp);
    const MetaData& get_meta_data() { return _meta_data; }
private:
    unsigned int _meta_data_len;
    MetaData _meta_data;
};

template <class META_DATA>
int CommonMetaReader<META_DATA>::read(FILE** fp) {
    if (NULL == fp || NULL == *fp) {
        ST_FATAL("file pointer can not be null.");
        return -1;
    }
    
    FILE *fptr = *fp;
    char *buffer = NULL;
    size_t read_count = 0;
    bool error = false;
    
    do {
        // meta data length;
        buffer = new (std::nothrow) char[sizeof(_meta_data_len)];
        if (NULL == buffer) {
            ST_FATAL("fail to allocate buffer.");
            error = true;
            break;
        }

        read_count = fread(buffer, sizeof(_meta_data_len), 1, fptr);
        if (1 != read_count) {
            ST_FATAL("fail to read meta data length.");
            error = true;
            break;
        }

        _meta_data_len = *(reinterpret_cast<unsigned int*>(buffer));
        delete[] buffer;
        buffer = NULL;

        // meta data.
        if (_meta_data_len != sizeof(META_DATA)) {
            ST_FATAL("meta data type is not correct.");
            error = true;
            break;
        }

        buffer = new (std::nothrow) char[_meta_data_len];
        if (NULL == buffer) {
            ST_FATAL("fail to allocate buffer.");
            error = true;
            break;
        }

        read_count = fread(buffer, sizeof(META_DATA), 1, fptr);
        if (1 != read_count) {
            ST_FATAL("fail to read meta data length.");
            error = true;
            break;
        }

        _meta_data = *(reinterpret_cast<META_DATA*>(buffer));
    } while(0);

    if (NULL != buffer) {
        delete[] buffer;
        buffer = NULL;
    }

    return error? -1 : 0;
}

typedef struct MetaData{
    static const int NAME_LEN = 256;
    static const int RESERVED_LEN = 256;
    
    int version;
    char name[NAME_LEN];
    int partition;
    unsigned long long item_size;
    unsigned long long item_count;
    char reserved[RESERVED_LEN];
    
    MetaData()
        : version(-1),
        partition(-1), 
        item_size(0), 
        item_count(0) {
        name[0] = '\0';
        reserved[0] = '\0';
    }

    void set_name(const char* n) {
        snprintf(name, NAME_LEN, "%s", n);
        return;
    }
} MetaData;

// dump table format.
// [meta data length][meta data][data]
typedef unsigned int MetaLenType;
const size_t META_LEN_LEN = sizeof(MetaLenType);
const size_t META_DATA_LEN = sizeof(MetaData);
const size_t META_HEADER_LEN = META_LEN_LEN + META_DATA_LEN;
    

} // namespace st

#endif // _META_READER_HPP_
