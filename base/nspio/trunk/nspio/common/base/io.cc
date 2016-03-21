// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/base/io.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <iostream>
#include <inttypes.h>
#include "base/io.h"


NSPIO_DECLARATION_START

// Copy copies from src to dst until either EOF is reached
// on src or an error occurs. It return the number of bytes
// copied and the first error encountered while copying.

int64_t io_Copy(io_Writer *dst, io_Reader *src) {
    int64_t size = 32 * 1024;
    char buf[size];
    int64_t nr, nw, written = 0;

    for (;;) {
	nr = src->Read(buf, size);
	if (nr > 0) {
	    nw = dst->Write(buf, nr);
	    if (nw > 0) {
		written += nw;
	    }
	    if (nr != nw) {
		break;
	    }
	}
	if (nr == -1) {
	    break;
	}
    }
    return written;
}

}
