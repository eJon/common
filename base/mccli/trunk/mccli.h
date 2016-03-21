#ifndef _MCCLI_H
#define _MCCLI_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <assert.h>
//#include "net.h"

typedef unsigned char u_char;

#define MCCLI_ERR -1
#define MCCLI_OK 0

#ifndef  MCCLI_MAX_VALUESIZE
#define MCCLI_MAX_VALUESIZE	1047552 //(1024*1024-1024) Max value size
 // MCServer Store:  item(48)+key(256)+flag(int)+expiretime(int)+valuesize(int)+tag <1M
#endif

#ifndef MCCLI_MAX_PACKET
#define MCCLI_MAX_PACKET 1048576 //(1024*1024)Memcached support max packet
#endif

#define MCCLI_ERR_IO 1
#define MCCLI_BLOCK 0x1
#define MCCLI_CONNECTED 0x2
#define MEM_ERROR -4
#define MEM_CLIENT_ERROR -3
#define MEM_SERVER_ERROR -2
#define MEM_SYSTEM_ERROR	-1
#define MEM_STORED 0
#define MEM_NOT_STORED 1
#define MEM_EXISTS 2
#define MEM_NOT_FOUND 3
#define MEM_DELETED 4
#define MEM_OK 5
#define MEM_NO_RESPONSE 6
#define CHECK_REPLY(msg, s, c ) if( strcmp( msg, s ) == 0 ) return c
#define NOTIFY_ERROR( err_code, desc ) \
	if( error_func != 0 ) \
		error_func( err_code, desc )

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _item_data
{
	char _key[256];
	u_char *_buffer;
	size_t _size;
	int _flag;
	size_t _expire;
} item_data;

typedef struct mcContext 
{
	int err;
	char errstr[768];
	int masternow;
	int fd;
	int flags;
	char mip[32];
	int mport;
	char slvip[32];
	int slvport;
	int srtimes;
	int maxsrtimes;
	int rctimes;
	int maxrctimes;
	struct timeval conntimeout;
	struct timeval socktimeout;

	char m_recvbuf[MCCLI_MAX_PACKET+1];  //Recv Buffer
	int m_currpos;		//current recv pos
	int m_parseflag;	//0:parse start
						//1:VALUE <key> <flags> <bytes> [<cas unique>]\r\n parse OK
						//2:<data block>\r\n parse OK
						//3:END\r\n parse OK

	int m_datapos;		//datablock start pos
	int m_size;			//reply msg size
	int m_flag;			//reply msg flag
} mcContext;

mcContext *mcConnectWithTimeout(const char *mip, int mport, const char *slvip, int slvport, struct timeval conntimeout, struct timeval srtimeout, int maxsrtimes, int maxrctimes);
void mcFree(mcContext *c);
void freeItem(item_data *item);

#define mc_add(server, item_ptr, need_response) mc_store(server, item_ptr, need_response, "add")
#define mc_set(server, item_ptr, need_response) mc_store(server, item_ptr, need_response, "set")
#define mc_replace(server, item_ptr, need_response) mc_store(server, item_ptr, need_response, "replace")
#define mc_append(server, item_ptr, need_response) mc_store(server, item_ptr, need_response, "append")
#define mc_prepend(server, item_ptr, need_response) mc_store(server, item_ptr, need_response, "prepend")
#define mc_inc(server, key, value, new_value) mc_inc_dec(server, key, value, new_value, "incr")
#define mc_dec(server, key, value, new_value) mc_inc_dec(server, key, value, new_value, "decr")

void *mc_get(mcContext *c, const char *key);
int mc_store(mcContext *c, const item_data *item, int need_response, const char *store_type);
int mc_delete(mcContext *c, const char key[256], int need_response);
int mc_inc_dec(mcContext *c, const char key[256], unsigned int value, unsigned int *new_value, const char *cmd_type);
int mc_stats(mcContext *c, char *stat_desc, size_t size );
int mc_flush_all(mcContext *c);

#ifdef __cplusplus
}
#endif

#endif
