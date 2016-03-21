// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Be used to dump CowTable.
// Author: wangqiushi@baidu.com
// Date: Jul 6 12:00:00 CST 2012

#include "dump_fn.h"

namespace st {

int DumpFn::init(const char* dir, const char* filename) {
    _filename.clear();
    _filename.append(dir).append("/").append(filename);

    _fp = fopen(_filename.c_str(), "wb");
    if (NULL == _fp) {
        ST_FATAL("fail to create file [%s].", _filename.c_str());
        return -1;
    }

    _buffer = new (std::nothrow) char[_item_size];
    if (NULL == _buffer) {
        ST_FATAL("fail to allcate buffer when dump file [%s].", _filename.c_str());
        return -1;
    }
    
    reset();

    return 0;
}

int DumpFn::reset() {
    _buffer[0] = '\0';
    _cur_pos = 0;

    return 0;
}

int DumpFn::write_meta_data(const MetaData& meta_data) {
    MetaLenType meta_len = static_cast<MetaLenType>(META_DATA_LEN);
    if (fwrite(&meta_len, sizeof(meta_len), 1, _fp) != 1) {
        if (ferror(_fp) != 0) {
            ST_FATAL("fail to writer meta data length into file [%s], error [%m].", _filename.c_str());
        } else if (feof(_fp) != 0) {
            ST_FATAL("fail to writer meta data length into file [%s], end of file.", _filename.c_str());
        } else {
            ST_FATAL("fail to writer meta data length into file [%s],"
                    " not end of file, no error.", _filename.c_str());
        }
        return -1;
    }
    
    if (fwrite(&meta_data, sizeof(meta_data), 1, _fp) != 1) {
        if (ferror(_fp) != 0) {
            ST_FATAL("fail to write meta data into file [%s], error [%m].", _filename.c_str());
        } else if (feof(_fp) != 0) {
            ST_FATAL("fail to write meta data into file [%s], end of file.", _filename.c_str());
        } else {
            ST_FATAL("fail to write meta data into file [%s],"
                    " not end of file, no error.", _filename.c_str());
        }
        return -1;
    }
    
    return 0;
}

int DumpFn::write() {
    if (fwrite(_buffer, _cur_pos, 1, _fp) != 1) {
        if (ferror(_fp) != 0) {
            ST_FATAL("fail to write data into file [%s], error [%m].", _filename.c_str());
        } else if (feof(_fp) != 0) {
            ST_FATAL("fail to write data into file [%s], end of file.", _filename.c_str());
        } else {
            ST_FATAL("fail to write data into file [%s],"
                    " not end of file, no error.", _filename.c_str());
        }
        return -1;
    }

    return 0;
}

int DumpFn::close() {
    if (NULL != _fp) {
        fclose(_fp);
        _fp = NULL;
    }

    if (NULL != _buffer) {
        delete[] _buffer;
        _buffer = NULL;
    }

    _cur_pos = 0;
    
    return 0;
}

} // namespace st
