// Copyright (c) 2010,2011 Baidu.com, Inc. All Rights Reserved
//
// Extend the standard errno.h for errors occurred in smalltable
// Author: gejun@baidu.com
// Date: Fri Sep 10 13:34:25 CST 2010

#ifndef _ST_ERRNO_H_
#define _ST_ERRNO_H_

// Most frequently used:
//   ENOMEM       Fail to new/malloc
//   EINVAL       Fail checking parameters
//   ENOTINIT     Calling functions of an uninitialized object
//   ECONFLICT    internal states are not sane
//   ERANGE       Value exceeds range

// Caution:
//   NEVER ASSUME THAT ERRNOS ARE NEGATIVE

#include <error.h>           // Inherit the standard errnos
#include <errno.h>
// Following lists SOME errnos
// E2BIG             Arg list too long
// EACCES            Permission denied
// EADDRINUSE        Address in use
// EADDRNOTAVAIL     Address not available
// EAFNOSUPPORT      Address family not supported
// EAGAIN            Resource temporarily unavailable
// EALREADY          Connection already in progress
// EBADF             Bad file descriptor
// EBADMSG           Bad message
// EBUSY             Resource busy
// ECANCELED         Operation canceled
// ECHILD            No child processes
// ECONNABORTED      Connection aborted
// ECONNREFUSED      Connection refused
// ECONNRESET        Connection reset
// EDEADLK           Resource deadlock avoided
// EDESTADDRREQ      Destination address required
// EDOM              Domain error
// EDQUOT            Reserved
// EEXIST            File exists
// EFAULT            Bad address
// EFBIG             File too large
// EHOSTUNREACH      Host is unreachable
// EIDRM             Identifier removed
// EILSEQ            Illegal byte sequence
// EINPROGRESS       Operation in progress
// EINTR             Interrupted function call
// EINVAL            Invalid argument
// EIO               Input/output error
// EISCONN           Socket is connected
// EISDIR            Is a directory
// ELOOP             Too many levels of symbolic links
// EMFILE            Too many open files
// EMLINK            Too many links
// EMSGSIZE          Inappropriate message buffer length
// EMULTIHOP         Reserved
// ENAMETOOLONG      Filename too long
// ENETDOWN          Network is down
// ENETRESET         Connection aborted by network
// ENETUNREACH       Network unreachable
// ENFILE            Too many open files in system
// ENOBUFS           No buffer space available
// ENODATA           No message is available on the STREAM head read queue
// ENODEV            No such device
// ENOENT            No such file or directory
// ENOEXEC           Exec format error
// ENOLCK            No locks available
// ENOLINK           Reserved
// ENOMEM            Not enough space
// ENOMSG            No message of the desired type
// ENOPROTOOPT       Protocol not available
// ENOSPC            No space left on device
// ENOSR             No STREAM resources
// ENOSTR            Not a STREAM
// ENOSYS            Function not implemented
// ENOTCONN          The socket is not connected
// ENOTDIR           Not a directory
// ENOTEMPTY         Directory not empty
// ENOTSOCK          Not a socket
// ENOTSUP           Not supported
// ENOTTY            Inappropriate I/O control operation
// ENXIO             No such device or address
// EOPNOTSUPP        Operation not supported on socket
// EOVERFLOW         Value too large to be stored in data type
// EPERM             Operation not permitted
// EPIPE             Broken pipe
// EPROTO            Protocol error
// EPROTONOSUPPORT   Protocol not supported
// EPROTOTYPE        Protocol wrong type for socket
// ERANGE            Result too large
// EROFS             Read-only file system
// ESPIPE            Invalid seek
// ESRCH             No such process
// ESTALE            Reserved
// ETIME             STREAM ioctl() timeout
// ETIMEDOUT         Operation timed out
// ETXTBSY           Test file busy
// EWOULDBLOCK       Operation would block (may be same value as EAGAIN)
// EXDEV             Improper link



// Caution: Please be conservative about adding new errno,
//          Look at existing errnos first
static const int EGENERAL     = 1000;      // hard-to-categorize errors, NOT recommended
static const int EIMMUTABLE   = 1003;      // Modify read-only data
static const int EEMPTY       = 1004;      // empty so that the some operation is denied
static const int EFULL        = 1005;      // full so that the some operation is denied
static const int EIMPOSSIBLE  = 1006;      // impossible by design
static const int EBADVERSION  = 1007;      // the version_id is less than previous one
static const int ENOTINIT     = 1008;      // not initialized
static const int ENOTEXIST    = 1009;      // not exist
static const int ECONFLICT    = 1010;      // conflict

#endif  //_ST_ERRNO_H_
