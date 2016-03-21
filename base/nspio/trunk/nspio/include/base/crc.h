// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/base/crc.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _CRC_H_
#define _CRC_H_

#include <inttypes.h>
#include "decr.h"

NSPIO_DECLARATION_START

uint64_t crc64(unsigned char *buf, uint32_t len);
uint16_t crc16(const char *buf, uint32_t len);

}

#endif
