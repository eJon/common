// copyright:
//            (C) SINA Inc.
//
//           file: nspio/include/nspio/errno.h
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


#ifndef _ERRORNO_INCLUDE_H_
#define _ERRORNO_INCLUDE_H_

#include <errno.h>

namespace nspio {

enum spio_errno {
    SPIO_ERRNO                                = 200,
    SPIO_ECONFIGURE                           = 201,
    SPIO_EROUTE                               = 202,
    SPIO_ECHECKSUM                            = 203,
    SPIO_ETIMEOUT                             = 204,
    SPIO_EREGISTRY                            = 205,
    SPIO_ENETUNREACH                          = 206,
    SPIO_EBADCONN                             = 207,
    SPIO_EDUPOP                               = 208,
    SPIO_ESPIODOWN                            = 209,
    SPIO_EINTERN                              = 210,
    SPIO_EQUEUEFULL                           = 211,
    SPIO_ENODISPATCHER                        = 212,
    SPIO_ERRNOEND,
};


}

#endif //_ERRORNO_INCLUDSPIO_EH_
