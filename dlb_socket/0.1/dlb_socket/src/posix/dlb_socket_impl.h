/************************************************************************
 * dlb_socket
 * Copyright (c) 2013, Dolby International AB.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************/

#ifndef dlb_socket_impl_H
#define dlb_socket_impl_H

/** @file dlb_socket_impl.h
 *  @brief posix specific part of socket implementation
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

typedef socklen_t size_type;
typedef socklen_t addr_size_type;
#define getLastError() (errno)
#define SOCKET_ERROR (-1)


/**
 * @brief initialize system-level socket library
 */
static inline
dlb_socket_res
dlb_socket_impl_init
    (int *err
    )
{
    *err = 0;
    return DLB_SOCKET_OK;
}


static inline
void
dlb_socket_impl_close
    (dlb_socket s
    )
{
    close(s);
}


static inline
dlb_socket
dlb_socket_impl_accept
    (dlb_socket listener
    ,struct sockaddr *addr
    ,addr_size_type *addrsize
    )
{
    return accept(listener, addr, addrsize);
}


static inline
void
dlb_socket_impl_reuseaddr
    (dlb_socket s
    )
{
    int optval = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}


static inline
void
dlb_socket_impl_setmode
    (dlb_socket s
    ,dlb_socket_mode mode
    )
{
    if (mode == DLB_SOCKET_NONBLOCKING)
    {
        int flags = fcntl(s, F_GETFL, 0);
        fcntl(s, F_SETFL, flags | O_NONBLOCK);
    }
}


static inline
void
dlb_socket_impl_getsockaddr
    (const char *ipaddr
    ,struct sockaddr_in *sockaddr
    ,int *sockaddr_size
    )
{
    (void)sockaddr_size;
    inet_aton(ipaddr, &sockaddr->sin_addr);
}


static inline
int
dlb_socket_impl_invalid
    (dlb_socket s
    )
{
    return s < 0;
}


static inline
int
dlb_socket_impl_would_block
    (void
    )
{
    return errno == EAGAIN || errno == EWOULDBLOCK;
}


static inline
dlb_socket_res
dlb_socket_impl_pair
    (int datagram
    ,dlb_socket *p_socket1
    ,dlb_socket *p_socket2
    ,int *err
    )
{
    int tmp[2];
    int r;

    r = socketpair(AF_LOCAL, datagram ? SOCK_DGRAM : SOCK_STREAM, 0, tmp);
    if (0 == r)
    {
        *p_socket1 = tmp[0];
        *p_socket2 = tmp[1];
        return DLB_SOCKET_OK;
    }
    else
    {
        *err = errno;
        return DLB_SOCKET_LIBRARY_ERR;
    }
}


#endif /*  dlb_socket_impl_H */
