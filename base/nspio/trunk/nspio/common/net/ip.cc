// copyright:
//            (C) SINA Inc.
//
//           file: nspio/common/net/ip.cc
//           desc: 
//  signed-off-by: Dong Fang <yp.fangdong@gmail.com>
//           date: 2014-02-14


/*
    Copyright (c) 2007-2011 iMatix Corporation
    Copyright (c) 2007-2011 Other contributors as noted in the AUTHORS file

    This file is part of 0MQ.

    2014/01/9 yp.fangdong@gmail.com
    ipaddr_atos()
    ipaddr_stoa()

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ifaddrs.h>
#include <iostream>
#include <sstream>
#include "net/ip.h"


NSPIO_DECLARATION_START

//  On these platforms, network interface name can be queried
//  using getifaddrs function.
static int resolve_nic_name (in_addr* addr_, char const *interface_)
{
    //  Get the addresses.
    ifaddrs* ifa = NULL;
    int rc = getifaddrs (&ifa);
    if (rc) {
		return -1;
    }

    //  Find the corresponding network interface.
    bool found = false;
    for (ifaddrs *ifp = ifa; ifp != NULL ;ifp = ifp->ifa_next)
        if (ifp->ifa_addr && ifp->ifa_addr->sa_family == AF_INET 
            && !strcmp (interface_, ifp->ifa_name)) 
        {
            *addr_ = ((sockaddr_in*) ifp->ifa_addr)->sin_addr;
            found = true;
            break;
        }

    //  Clean-up;
    freeifaddrs (ifa);

    if (!found) {
        errno = ENODEV;
        return -1;
    }

    return 0;
}

int resolve_ip_interface (sockaddr_storage* addr_, socklen_t *addr_len_,
    char const *interface_)
{
    //  Find the ':' at end that separates NIC name from service.
    const char *delimiter = strrchr (interface_, ':');
    if (!delimiter) {
        errno = EINVAL;
        return -1;
    }

    //  Separate the name/port.
    std::string iface (interface_, delimiter - interface_);
    std::string service (delimiter + 1);

    //  Initialize the output parameter.
    memset (addr_, 0, sizeof (*addr_));

    //  Initialise IPv4-format family/port.
    sockaddr_in ip4_addr;
    memset (&ip4_addr, 0, sizeof (ip4_addr));
    ip4_addr.sin_family = AF_INET;
    ip4_addr.sin_port = htons ((uint16_t) atoi (service.c_str()));

    //  Initialize temporary output pointers with ip4_addr
    sockaddr *out_addr = (sockaddr *) &ip4_addr;
    uint32_t out_addrlen = sizeof (ip4_addr);

    //  0 is not a valid port.
    if (!ip4_addr.sin_port) {
        errno = EINVAL;
        return -1;
    }

    //  * resolves to INADDR_ANY.
    if (iface.compare("*") == 0) {
        ip4_addr.sin_addr.s_addr = htonl (INADDR_ANY);
        memcpy (addr_, out_addr, out_addrlen);
        *addr_len_ = out_addrlen;
        return 0;
    }

    //  Try to resolve the string as a NIC name.
    int rc = resolve_nic_name (&ip4_addr.sin_addr, iface.c_str());
    if (rc != 0 && errno != ENODEV)
        return rc;
    if (rc == 0) {
        memcpy (addr_, out_addr, out_addrlen);
        *addr_len_ = out_addrlen;
        return 0;
    }

    //  There's no such interface name. Assume literal address.
    addrinfo *res = NULL;
    addrinfo req;
    memset (&req, 0, sizeof (req));

    //  We only support IPv4 addresses for now.
    req.ai_family = AF_INET;

    //  Arbitrary, not used in the output, but avoids duplicate results.
    req.ai_socktype = SOCK_STREAM;

    //  Restrict hostname/service to literals to avoid any DNS lookups or
    //  service-name irregularity due to indeterminate socktype.
    req.ai_flags = AI_PASSIVE | AI_NUMERICHOST | AI_NUMERICSERV;

    //  Resolve the literal address. Some of the error info is lost in case
    //  of error, however, there's no way to report EAI errors via errno.
    rc = getaddrinfo (iface.c_str(), service.c_str(), &req, &res);
    if (rc) {
        errno = ENODEV;
        return -1;
    }

    //  Use the first result.
    memcpy (addr_, res->ai_addr, res->ai_addrlen);
    *addr_len_ = res->ai_addrlen;

    //  Cleanup getaddrinfo after copying the possibly referenced result.
    if (res)
        freeaddrinfo (res);

    return 0;
}

int resolve_ip_hostname (sockaddr_storage *addr_, socklen_t *addr_len_,
    const char *hostname_)
{
    //  Find the ':' that separates hostname name from service.
    const char *delimiter = strchr (hostname_, ':');
    if (!delimiter) {
        errno = EINVAL;
        return -1;
    }

    //  Separate the hostname and service.
    std::string hostname (hostname_, delimiter - hostname_);
    std::string service (delimiter + 1);

    //  Set up the query.
    addrinfo req;
    memset (&req, 0, sizeof (req));

    //  We only support IPv4 addresses for now.
    req.ai_family = PF_INET;

    //  Need to choose one to avoid duplicate results from getaddrinfo() - this
    //  doesn't really matter, since it's not included in the addr-output.
    req.ai_socktype = SOCK_STREAM;
    
    //  Avoid named services due to unclear socktype.
    req.ai_flags = AI_NUMERICSERV;

    //  Resolve host name. Some of the error info is lost in case of error,
    //  however, there's no way to report EAI errors via errno.
    addrinfo *res;
    int rc = getaddrinfo (hostname.c_str (), service.c_str (), &req, &res);
    if (rc) {
        errno = EINVAL;
        return -1;
    }

    //  Copy first result to output addr with hostname and service.
    memcpy (addr_, res->ai_addr, res->ai_addrlen);
    *addr_len_ = res->ai_addrlen;
 
    freeaddrinfo (res);
    
    return 0;
}

int resolve_local_path (sockaddr_storage *addr_, socklen_t *addr_len_,
    const char *path_)
{
  sockaddr_un *un = (sockaddr_un*) addr_;
  if (strlen (path_) >= sizeof (un->sun_path))
  {
    errno = ENAMETOOLONG;
    return -1;
  }
  strcpy (un->sun_path, path_);
  un->sun_family = AF_UNIX;
  *addr_len_ = sizeof (sockaddr_un);
  return 0;
}




int ipaddr_atos(uint8_t ip[4], uint16_t port, string &ips) {
    stringstream ss;

    ss << (int)ip[0] << "." << (int)ip[1] << "." << (int)ip[2] << "." << (int)ip[3] << ":";
    ss << (int)port;
    ips = ss.str();
    return 0;
}


int ipaddr_stoa(const char *ips, uint8_t ip[4], uint16_t *port) {
    int _ip[4] = {}, _port = 0;
    char *dupip = strdup(ips), *end = NULL, *pos = dupip;
    if (!dupip)
	return -1;
    end = pos + strlen(ips);
    while (pos <= end) {
	if (pos[0] == '.' || pos[0] == ':')
	    pos[0] = ' ';
	pos++;
    }
    sscanf(dupip, "%d %d %d %d %d", &_ip[0], &_ip[1], &_ip[2], &_ip[3], &_port);
    ip[0] = _ip[0] & 0xff;
    ip[1] = _ip[1] & 0xff;
    ip[2] = _ip[2] & 0xff;
    ip[3] = _ip[3] & 0xff;
    *port = _port & 0xffff;
    free(dupip);
    return 0;
}




}
