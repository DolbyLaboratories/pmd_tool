/************************************************************************
 * dlb_socket
 * Copyright (c) 2020, Dolby Laboratories Inc.
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
 *  @brief windows specific part of socket implementation
 */

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>

#if defined(_MSC_VER) && !defined(inline)
#define inline __inline
#endif

typedef int size_type;
typedef int addr_size_type;
#define getLastError() (WSAGetLastError())

static int g_sockets_opened = 0;


/**
 * @brief initialize system-level socket library
 */
static inline
dlb_socket_res
dlb_socket_impl_init
    (int *err
    )
{
    if (0 == g_sockets_opened)
    {
        WSADATA wsa;
        int res = WSAStartup(MAKEWORD(2,2), &wsa);
        if (res != 0)
        {
            *err = res;
            return DLB_SOCKET_LIBRARY_ERR;
        }
    }
    ++g_sockets_opened;
    return DLB_SOCKET_OK;
}


static inline
SOCKET
dlb_socket_impl_accept
    (SOCKET listener
    ,struct sockaddr *addr
    ,int *addrsize
    )
{
    SOCKET s = accept(listener, addr, addrsize);
    if (s != INVALID_SOCKET)
    {
        ++g_sockets_opened;
    }
    return s;
}



static inline
void
dlb_socket_impl_close
    (dlb_socket s
    )
{
    if (INVALID_SOCKET != s)
    {
        closesocket(s);

        --g_sockets_opened;
        if (0 == g_sockets_opened)
        {
            WSACleanup();   
        }
    }
}




static inline
void
dlb_socket_impl_reuseaddr
    (dlb_socket s
    )
{
    int reuse = 1;
    (void)setsockopt(s, SOL_SOCKET, SO_REUSEADDR,
                     (char *)&reuse, (socklen_t)sizeof(reuse));
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
      u_long i_mode = 1;
      ioctlsocket(s, FIONBIO, &i_mode);
   }
   else
   {
      u_long i_mode = 0;
      ioctlsocket(s, FIONBIO, &i_mode);
   }
}





static inline
void
dlb_socket_impl_getsockaddr
    (char *ipaddr
    ,struct sockaddr_in *sockaddr
    ,int *sockaddr_size
    )
{
    WSAStringToAddress(ipaddr, AF_INET, NULL, (LPSOCKADDR)&sockaddr,
                       sockaddr_size);
}





static inline
int
dlb_socket_impl_invalid
    (dlb_socket s
    )
{
    return s == INVALID_SOCKET;
}



static inline
int
dlb_socket_impl_would_block
    ()
{
    return WSAGetLastError() == WSAEWOULDBLOCK;
}






dlb_socket_res
dlb_socket_impl_pair
    (int datagram
    ,dlb_socket *p_socket1
    ,dlb_socket *p_socket2
    ,int *err
    )
{
    struct sockaddr_in inaddr;
    SOCKET listener;
    int addrlen = sizeof(inaddr);
    int type = datagram ? SOCK_DGRAM : SOCK_STREAM;

    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == listener)
    {
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
    }

    memset(&inaddr, '\0', sizeof(inaddr));
    inaddr.sin_family = AF_INET;
    inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    inaddr.sin_port = 0;

    *p_socket1 = INVALID_SOCKET;
    *p_socket2 = INVALID_SOCKET;

    dlb_socket_impl_reuseaddr(listener);

    if  ((SOCKET_ERROR == bind(listener, (struct sockaddr *)&inaddr, sizeof(inaddr))) ||
        ((SOCKET_ERROR == getsockname(listener, (struct sockaddr *)&inaddr, &addrlen))) ||
        ((SOCKET_ERROR == listen(listener, 1))))
    {
        goto fail;
    }
    *p_socket1 = socket(AF_INET, type, 0);
    if (dlb_socket_impl_invalid(*p_socket1))
    {
        goto fail;
    }
    if (SOCKET_ERROR == connect(*p_socket1, (struct sockaddr *)&inaddr, sizeof(inaddr)))
    {
        goto fail2;
    }
    *p_socket2 = accept(listener, NULL, NULL);
    if (dlb_socket_impl_invalid(*p_socket2))
    {
        goto fail2;
    }
    
    closesocket(listener);
    return DLB_SOCKET_OK;

  fail2:
    closesocket(*p_socket1);
  fail:
    closesocket(listener);
    *err = getLastError();
    return DLB_SOCKET_LIBRARY_ERR;
}


        

#endif /*  dlb_socket_impl_H */
