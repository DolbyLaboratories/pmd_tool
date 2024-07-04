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

/** @file dlb_socket.c
 *  @brief Thin wrapper on system dependent sockets - implementation
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "dlb_socket/include/dlb_socket.h"
#include "dlb_socket_impl.h"

#ifdef _MSC_VER
#  define snprintf _snprintf
#endif

/**
 * @brief generic open socket function
 */
static
dlb_socket_res
dlb_open_socket
    (dlb_socket* p_socket
    ,const char* iface
    ,int local_port
    ,int sock_type
    ,int* err
    )
{
    dlb_socket_res res;
    struct sockaddr_in local_node;
    int local_node_size = sizeof(local_node);

    if (NULL == p_socket)
    {
        return DLB_SOCKET_INVALID_ARG;
    }

    res = dlb_socket_impl_init(err);
    if (DLB_SOCKET_OK != res)
    {
        return res;
    }

    /* Create socket */
    *p_socket = socket(AF_INET, sock_type, 0);
    if (dlb_socket_impl_invalid(*p_socket))
    {
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
    }

    local_node.sin_family = AF_INET;
    local_node.sin_addr.s_addr = (iface == NULL)
        ? INADDR_ANY
        : inet_addr(iface);
    local_node.sin_port = htons(local_port);

    dlb_socket_impl_reuseaddr(*p_socket);

    if (bind(*p_socket
             ,(struct sockaddr *)&local_node
             ,local_node_size) == SOCKET_ERROR)
    {
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
    }

    return DLB_SOCKET_OK;
}


dlb_socket_res
dlb_socket_open_datagram
    (dlb_socket* p_socket
    ,char *iface
    ,int local_port
    ,int* err
    )
{
    return dlb_open_socket(p_socket, iface, local_port, SOCK_DGRAM, err);
}


dlb_socket_res
dlb_socket_create_stream_server
    (dlb_socket* p_socket
    ,const char* iface
    ,int local_port
    ,int* err)
{
    dlb_socket_res res =
        dlb_open_socket(p_socket, iface, local_port, SOCK_STREAM, err);
    if (DLB_SOCKET_OK == res)
    {
        listen(*p_socket, 1);
    }
    return res;
}


dlb_socket_res
dlb_socket_accept
    (dlb_socket socket
    ,dlb_socket* p_socket_client
    ,dlb_socket_mode mode
    ,char (*address)[DLB_SOCKET_MAX_ADDRESS_SIZE]
    ,int* err)
{
   struct sockaddr_in addr;
   addr_size_type addr_size = sizeof(addr);

   if (dlb_socket_impl_invalid(socket) || (NULL == p_socket_client))
   {
      return DLB_SOCKET_INVALID_ARG;
   }

   dlb_socket_impl_setmode(socket, mode);

   *p_socket_client = dlb_socket_impl_accept(socket, (struct sockaddr*)&addr, &addr_size);
   if (dlb_socket_impl_invalid(*p_socket_client))
   {
       /* printf("Accept error : %d\n", getLastError()); */
       *err = getLastError();
       return DLB_SOCKET_LIBRARY_ERR;
   }

   /* if caller requested other side address */
   if (address != NULL)
   {
       strncpy(*address, inet_ntoa(addr.sin_addr),
               DLB_SOCKET_MAX_ADDRESS_SIZE);
   }
   return DLB_SOCKET_OK;
}


dlb_socket_res
dlb_socket_connect
    (dlb_socket* p_socket
    ,char* other_address
    ,int other_port
    ,char* iface
    ,int* err
    )
{
//    struct sockaddr_in other_node;
//    int other_node_size = sizeof(other_node);
    dlb_socket_res res;    
    char service[32];

    struct addrinfo hints;
    struct addrinfo *server_info;
    struct addrinfo *si;

    if ((NULL == p_socket) || (NULL == other_address))
    {
        return DLB_SOCKET_INVALID_ARG;
    }

    res = dlb_socket_impl_init(err);
    if (DLB_SOCKET_OK != res)
    {
        return res;
    }

    /* Create socket */
    *p_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (dlb_socket_impl_invalid(*p_socket))
    {
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
    }

    if (NULL != iface && 0 != strncmp(iface, "0.0.0.0", 8))
    {
        /* connect via given interface */
        struct sockaddr_in siface;
        memset(&siface, 0, sizeof(siface));
        siface.sin_family = AF_INET;
        siface.sin_addr.s_addr = inet_addr(iface);
        if (bind(*p_socket, (struct sockaddr*)&siface, sizeof(siface)) == SOCKET_ERROR)
        {
            *err = getLastError();
            return DLB_SOCKET_LIBRARY_ERR;
        }
    }

#if 0
    /* Try to connect to the server */
    dlb_socket_impl_getsockaddr(other_address, &other_node,
                                &other_node_size);
    other_node.sin_family = AF_INET;
    other_node.sin_addr.s_addr = inet_addr(other_address);
    other_node.sin_port = htons(other_port);

    if (connect(*p_socket, (const struct sockaddr*)&other_node, other_node_size) != 0)
    {
        *err = getLastError();
        dlb_socket_impl_close(*p_socket);
        return DLB_SOCKET_LIBRARY_ERR;
    }
#else
    /* Try to connect to the server */
    /* resolve hostname */
    memset(&hints, '\0', sizeof(hints));
    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCK_STREAM;
    hints.ai_flags     = AI_CANONNAME;
    hints.ai_protocol  = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    
    snprintf(service, sizeof(service), "%d", other_port);
    *err = getaddrinfo(other_address, service, &hints, &server_info);
    if (0 != *err)
    {
        //printf("failed to resolve hostname %s: %s\n", name, gai_strerror(err));
        return DLB_SOCKET_LIBRARY_ERR;
    }
    
    res = DLB_SOCKET_LIBRARY_ERR;
    for (si = server_info; NULL != si; si = si->ai_next)
    {
        if (0 == connect(*p_socket, si->ai_addr, si->ai_addrlen))
        {
            res = DLB_SOCKET_OK;
            break;
        }
        *err = getLastError();
    }
    freeaddrinfo(server_info);
#endif

    return res;
}


dlb_socket_res
dlb_socket_connect_sockaddr
    (dlb_socket* p_socket
    ,struct sockaddr* addr
    ,socklen_t len
    ,int* err
    )
{
     if ((NULL == p_socket) || (NULL == addr))
     {
         return DLB_SOCKET_INVALID_ARG;
     }

     /* Create socket */
     *p_socket = socket(AF_INET, SOCK_STREAM, 0);
     if (dlb_socket_impl_invalid(*p_socket))
     {
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
     }

     if (connect(*p_socket, addr, len) != 0)
     {
        *err = getLastError();
        dlb_socket_impl_close(*p_socket);
        return DLB_SOCKET_LIBRARY_ERR;
     }

     return DLB_SOCKET_OK;
}


dlb_socket_res
dlb_socket_close
    (dlb_socket socket
    )
{
    if (dlb_socket_impl_invalid(socket))
    {
        return DLB_SOCKET_INVALID_ARG;
    }

    dlb_socket_impl_close(socket);
    return DLB_SOCKET_OK;
}


dlb_socket_res
dlb_socket_select
    (dlb_socket* readers
    ,dlb_socket* writers
    ,int num_readers
    ,int num_writers
    ,int timeout_msec
    ,int* num_sockets
    ,int* err
    )
{
    struct timeval timeval;
    struct timeval *timeout;
    fd_set readset;
    fd_set writeset;
    int maxfd = -1;
    int sel_res;
    int i;

    if (num_readers < 0
        || num_writers < 0
        || num_sockets == NULL
        || err == NULL)
    {
        return DLB_SOCKET_INVALID_ARG;
    }

    FD_ZERO(&readset);
    FD_ZERO(&writeset);

    for (i = 0; i != num_readers; ++i)
    {
        FD_SET(readers[i], &readset);
        if (maxfd < readers[i])
        {
            maxfd = readers[i];
        }
    }
    for (i = 0; i != num_writers; ++i)
    {
        FD_SET(writers[i], &readset);
        if (maxfd < writers[i])
        {
            maxfd = writers[i];
        }
    }

    if (timeout_msec == DLB_SOCKET_WAIT_FOREVER)
    {
        timeout = NULL;
    }
    else
    {
        timeval.tv_sec = timeout_msec / 1000;
        timeout_msec -= timeval.tv_sec * 1000;
        timeval.tv_usec = timeout_msec * 1000;
        timeout = &timeval;
    }

    *num_sockets = 0;
    sel_res = select(maxfd+1, &readset, &writeset, NULL, timeout);
    if (0 == sel_res)
    {
        return DLB_SOCKET_NO_DATA;
    }
    else if (sel_res < 0)
    {
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
    }
    else
    {
        *num_sockets = sel_res;
    }
    return DLB_SOCKET_OK;
}


dlb_socket_res
dlb_socket_read
    (dlb_socket socket
    ,void* buff
    ,size_t buff_len
    ,ssize_t *res_len
    ,dlb_socket_mode mode
    ,int* err)
{
    struct sockaddr_in from_addr;
    size_type from_addr_len = sizeof(from_addr);
    int flags = 0;

    if (dlb_socket_impl_invalid(socket) || (NULL == buff) || (NULL == res_len))
    {
        return DLB_SOCKET_INVALID_ARG;
    }

    dlb_socket_impl_setmode(socket, mode);

    if ((*res_len = recvfrom(socket
                             ,(char*)buff
                             ,buff_len
                             ,flags
                             ,(struct sockaddr *) &from_addr
                             ,&from_addr_len) ) == SOCKET_ERROR)
    {
        if (mode == DLB_SOCKET_NONBLOCKING
            && dlb_socket_impl_would_block())
        {
            /* TODO: handle partial reads possible in non-blocking scenario */
            *res_len = 0;
            return DLB_SOCKET_NO_DATA;
        }
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
    }
    return DLB_SOCKET_OK;
}


dlb_socket_res
dlb_socket_stream_write
    (dlb_socket socket
    ,void* buff
    ,size_t send_len
    ,int* err)
{
    int res, flags = 0;

    if (dlb_socket_impl_invalid(socket) || (NULL == buff))
    {
        return DLB_SOCKET_INVALID_ARG;
    }

#ifdef MSG_NOSIGNAL
    flags = MSG_NOSIGNAL;
#endif
    res = send(socket, (const char*)buff, send_len, flags);
    if (res == SOCKET_ERROR)
    {
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
    }

    return DLB_SOCKET_OK;
}


dlb_socket_res
dlb_socket_datagram_write
    (dlb_socket socket
    ,char* other_address
    ,int other_port
    ,void* buff
    ,size_t send_len
    ,int* err)
{
    struct sockaddr_in other_node;
    int other_node_size = sizeof(other_node);

    if (dlb_socket_impl_invalid(socket) || (NULL == buff) || (NULL == other_address))
    {
        return DLB_SOCKET_INVALID_ARG;
    }

    /* Prepare the sockaddr_in structure */
    dlb_socket_impl_getsockaddr(other_address, &other_node,
                                &other_node_size);
    other_node.sin_family = AF_INET;
    other_node.sin_port = htons(other_port);

    if (sendto(socket, (char*)buff, send_len, 0, /*flags*/
               (struct sockaddr *)&other_node,
               other_node_size) == SOCKET_ERROR)
    {
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
    }

    return DLB_SOCKET_OK;
}


int
dlb_socket_valid
    (dlb_socket socket
     )
{
    return !dlb_socket_impl_invalid(socket);
}


dlb_socket_res
dlb_socket_set_rcvbuf
    (dlb_socket socket
    ,size_t bytes
    ,int* err
    )
{
    int res;

    if (dlb_socket_impl_invalid(socket))
    {
        return DLB_SOCKET_INVALID_ARG;
    }

    res = setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &bytes, (socklen_t)sizeof(bytes));
    if (res != 0)
    {
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
    }

    return DLB_SOCKET_OK;
}


dlb_socket_res
dlb_socket_join_mcast_group
    (dlb_socket socket
    ,char* group_addr
    ,char* iface
    ,char* igmp_src_addr
    ,int* err
    )
{
    int res;

    if (dlb_socket_impl_invalid(socket) || NULL == group_addr)
    {
        return DLB_SOCKET_INVALID_ARG;
    }

    if (NULL == igmp_src_addr)
    {
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = inet_addr(group_addr);
        mreq.imr_interface.s_addr = (iface == NULL)
            ? INADDR_ANY
            : inet_addr(iface);

        res = setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         (char*)&mreq, sizeof(mreq));
    }
    else
    {
        struct ip_mreq_source mreq;
        mreq.imr_multiaddr.s_addr = inet_addr(group_addr);
        mreq.imr_interface.s_addr = (iface == NULL)
            ? INADDR_ANY
            : inet_addr(iface);
        mreq.imr_sourceaddr.s_addr = inet_addr(igmp_src_addr);

        res = setsockopt(socket, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP,
                         (char*)&mreq, sizeof(mreq));
    }

    if (res != 0)
    {
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
    }
    return DLB_SOCKET_OK;
}


dlb_socket_res
dlb_socket_leave_mcast_group
    (dlb_socket socket
    ,char* group_addr
    ,int* err
    )
{
    /* TODO: use ip_mreq_source and IP_DROP_SOURCE_MEMBERSHIP for IGMP3? */
    struct ip_mreq mreq;
    int res;

    if (dlb_socket_impl_invalid(socket) || NULL == group_addr)
    {
        return DLB_SOCKET_INVALID_ARG;
    }

    mreq.imr_multiaddr.s_addr = inet_addr(group_addr);
    mreq.imr_interface.s_addr = INADDR_ANY;

    res = setsockopt(socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                     (char*)&mreq, sizeof(mreq));
    if (res != 0)
    {
        *err = getLastError();
        return DLB_SOCKET_LIBRARY_ERR;
    }
    return DLB_SOCKET_OK;
}


dlb_socket_res
dlb_socket_pair
    (int datagram
    ,dlb_socket *p_socket1
    ,dlb_socket *p_socket2
    ,int *err
    )
{
    int res;

    if (NULL == p_socket1 || NULL == p_socket2 || NULL == err)
    {
        return DLB_SOCKET_INVALID_ARG;
    }

    res = dlb_socket_impl_init(err);
    if (DLB_SOCKET_OK != res)
    {
        return res;
    }

    return dlb_socket_impl_pair(datagram, p_socket1, p_socket2, err);
}
