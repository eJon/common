// copyright:
//            (C) SINA Inc.
//
//           file: nspio/core/sectionreader.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#include <stdio.h>
#include <nspio/errno.h>
#include "sectionreader.h"
#include "log.h"


NSPIO_DECLARATION_START

int SectionReadWriter::ReadSection(Conn *conn, char *rbuffer) {
    int ret;
    if (!rbuffer || !conn) {
	NSPIOLOG_ERROR("Invalid conn or rbuffer");
	return -1;
    }

    while (rindex < rlen) {
	if ((ret = conn->Read(rbuffer + rindex, rlen - rindex)) < 0) {
	    NSPIOLOG_ERROR("Read section: %u of %u", rindex, rlen);
	    return -1;
	}
	rindex += ret;
    }

    ResetReader();
    return 0;
}

int SectionReadWriter::WriteSection(Conn *conn, char *wbuffer) {
    int ret;
    if (!conn || !wbuffer) {
	NSPIOLOG_ERROR("Invalid conn or wbuffer");
	return -1;
    }

    while (windex < wlen) {
	if ((ret = conn->Write(wbuffer + windex, wlen - windex)) < 0) {
	    NSPIOLOG_ERROR("Write section: %u of %u", windex, wlen);
	    return -1;
	}
	windex += ret;
    }

    ResetWriter();
    return 0;
}



}
