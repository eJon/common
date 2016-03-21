// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/base/io.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _SLB_IO_H_
#define _SLB_IO_H_

#include <inttypes.h>
#include "decr.h"

NSPIO_DECLARATION_START

class io_Reader {
 public:
    io_Reader() {}
    virtual ~io_Reader() {

    }
    virtual int64_t Read(char *buf, int64_t len) = 0;
};
 
class io_Writer {
 public:
    io_Writer() {}
    virtual ~io_Writer() {

    }
    virtual int64_t Write(char *buf, int64_t len) = 0;
};

class io_ReadWriter {
 public:
    io_ReadWriter() {}
    virtual ~io_ReadWriter() {

    }
    virtual int64_t Read(char *buf, int64_t len) = 0;
    virtual int64_t Write(char *buf, int64_t len) = 0;
};


int64_t io_Copy(io_Writer *dst, io_Reader *src);


}
 
#endif
