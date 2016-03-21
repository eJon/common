// Copyright(c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Binary file reader.
// Author: wangqiushi@baidu.com
// Date: Jul 6 12:00:00 CST 2012

#include "binary_file_reader.h"

namespace st {

int BinaryFileReader::init(int read_item_once) {
    if (_item_size <= 0) {
        return -1;
    }

    _buffer_len = _item_size * read_item_once;
    _buffer = new (std::nothrow) char[_buffer_len];
    if (NULL == _buffer) {
        ST_FATAL("fail to allcate buffer when read file [%s].", _filename.c_str());
        return -1;
    }

    _item_count = 0;
    _cur_item = NULL;

    return 0;
}

int BinaryFileReader::close() {
    if (NULL != _fp) {
        fclose(_fp);
        _fp = NULL;
    }

    if (NULL != _buffer) {
        delete[] _buffer;
        _buffer = NULL;
    }
    _buffer_len = 0;

    _item_count = 0;

    _cur_item = NULL;
    
    return 0;
}

BinaryFileReader::ReadState BinaryFileReader::reset_buffer() {
    size_t max_item_count = _buffer_len / _item_size;
    _item_count = fread(_buffer, _item_size, max_item_count, _fp);

    if (_item_count < max_item_count) {
        if (ferror(_fp) != 0) {
            ST_FATAL("fail to read data from file [%s], error [%m].", _filename.c_str());
            return BinaryFileReader::FAIL;
        }
    }
    
    if (0 == _item_count) { // end of file.
        _cur_item = NULL;
        return END;
    } else {
        _cur_item = _buffer;
        return SUCCEED;
    }
}

BinaryFileReader::ReadState BinaryFileReader::read_next() {
    if (NULL == _cur_item) { // no more data.
        return END;
    }

    ReadState state = SUCCEED;
    if ((_buffer + (_item_count * _item_size)) == (_cur_item + _item_size)) {   // read more data.
        switch(reset_buffer()) {
            case END:
                state = END;
                break;
            case FAIL:
                ST_FATAL("fail to read data from file [%s].", _filename.c_str());
                _cur_item = NULL;
                state= FAIL;
                break;
            default:
                break;
        }
    } else {
        _cur_item += _item_size;
    }
    
    return state;
}

} // namespace st
