/************************************************************************
 * dlb_socket
 * Copyright (c) 2021, Dolby Laboratories Inc.
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

/** @ingroup ds */

/** @file dlb_socket.h
 *  @brief Thin wrapper on system dependent sockets - API
 */

#ifndef dlb_socket_H
#define dlb_socket_H

#include <stddef.h>
#include "dlb_socket_os_impl.h"

typedef dlb_socket_os_impl dlb_socket;


/**
 * @def DLB_SOCKET_MAX_ADDRESS_SIZE
 * @brief size of char array used to store IP addresses
 */
#define DLB_SOCKET_MAX_ADDRESS_SIZE (32)


/**
 * @def DLB_SOCKET_WAIT_FOREVER
 * @brief indicate an infinite timeout
 */
#define DLB_SOCKET_WAIT_FOREVER (-1)


/**
 * @brief return types for socket operations
 */
typedef enum
{
    DLB_SOCKET_OK = 0,          /**< socket operation succeeded */
    DLB_SOCKET_INVALID_ARG = 1, /**< invalid socket operation arguments */
    DLB_SOCKET_LIBRARY_ERR = 2, /**< socket library failed */
    DLB_SOCKET_NO_DATA = 3      /**< no data available to read */
} dlb_socket_res;


/**
 * @brief socket creation flags
 */
typedef enum
{
    DLB_SOCKET_NONBLOCKING = 0,   /**< create a non-blocking socket */
    DLB_SOCKET_BLOCKING = 1       /**< create a blocking socket */
} dlb_socket_mode;


/**
 * @brief create a (TCP) stream server socket
 */
dlb_socket_res /** @return DLB_SOCKET_INVALID_ARG if arguments are wrong
                *          DLB_SOCKET_LIBRARY_ERR if system could not create
                *          DLB_SOCKET_OK          on success 
                */
dlb_socket_create_stream_server
    (dlb_socket* p_socket   /**< [out] socket to create */
    ,const char* iface      /**< [in] interface to listen on, NULL for default */
    ,int local_port         /**< [in] local port to listen on */
    ,int* err               /**< [out] sytem error code upon failure */
    );


/**
 * @brief accept a stream socket connection on given server socket
 *
 * This call is used on the server-side to accept a connection from a
 * client.  Typically a server writer will block waiting for activity
 * on the listening socket via select call, and then accept.
 *
 * The sole purpose of a stream server socket (as might be used in a
 * HTTP server say) simply waits for connections from the outside
 * world.  The moment one is accepted, a new socket must be generated
 * to process the traffic between the client and the server.
 *
 * For instance, if the server socket belonged to a web server, the
 * accept call would generate a new socket for the purposes of
 * delivering the web-page to the client.
 */
dlb_socket_res                     /** @return DLB_SOCKET_INVALID_ARG
                                    *          DLB_SOCKET_LIBRARY_ERR
                                    *        or DLB_SOCKET_OK
                                    */
dlb_socket_accept
    (dlb_socket socket             /**< [in] listening server socket */
    ,dlb_socket* p_socket_client   /**< [out] new client socket */
    ,dlb_socket_mode mode          /**< [in] will server block on client or not */
    ,char (*address)[DLB_SOCKET_MAX_ADDRESS_SIZE]
                                   /**< [out] return client IP address if not null*/
    ,int* err                      /**< [out] system-level error code on failure */
    );


/**
 * @brief attempt to connect to a stream server with ip/port
 *
 * This call is used by a client wishing to request services from a
 * server.  For instance, if we were writing a web-browser, we should
 * use this call to attempt connection to a given web-server address.
 *
 * This call will create a socket. 
 *
 * @note DNS resolution should be done outside this call
 *
 * @note both Stream (TCP) and datagram (UDP) sockets can connect
 */
dlb_socket_res                     /** @return DLB_SOCKET_INVALID_ARG
                                    *          DLB_SOCKET_LIBRARY_ERR
                                    *        or DLB_SOCKET_OK
                                    */
dlb_socket_connect
    (dlb_socket* p_socket          /**< [out] client socket to connect */
    ,char* server_address          /**< [in] target server IP address */
    ,int server_port               /**< [in] target server port */
    ,char* iface                   /**< [in] interface address or NULL for default */
    ,int* err                      /**< [out] system-level error code */
    );


/**
 * @brief attempt to connect to a stream server with a sockaddr
 *
 * This call is used by a client wishing to request services from a
 * server.  For instance, if we were writing a web-browser, we should
 * use this call to attempt connection to a given web-server address.
 *
 * This call will create a socket.
 *
 * @note DNS resolution should be done outside this call
 *
 * @note both Stream (TCP) and datagram (UDP) sockets can connect
 */
dlb_socket_res                     /** @return DLB_SOCKET_INVALID_ARG
                                    *          DLB_SOCKET_LIBRARY_ERR
                                    *        or DLB_SOCKET_OK
                                    */
dlb_socket_connect_sockaddr
    (dlb_socket* p_socket          /**< [out] client socket to connect */
    ,struct sockaddr* addr         /**< [in] target server sockaddr */
    ,socklen_t len                 /**< [in] target server sockaddr length */
    ,int* err                      /**< [out] system-level error code on failure */
    );


/**
 * @brief open a connectionless, datagram (UDP) client socket
 *
 * A stream socket, or TCP socket, is a _reliable_ communication
 * protocol, in that it guarantees to send all data in the correct
 * order while the connection stays open.  However, this comes at a
 * price: the protocol is burdened with transmission error handling
 * and retries, and this makes it much slower.
 *
 * For certain applications, such as telephony or video streaming,
 * sometimes a faster protocol is required with lighter overhead,
 * where lost data can be tolerated more readily.  The UDP protocol
 * delivers packets independently, and each packet can be lost, routed
 * differently through the network and be delivered out-of-order from
 * the others.
 */
dlb_socket_res                     /** @return DLB_SOCKET_INVALID_ARG
                                    *          DLB_SOCKET_LIBRARY_ERR
                                    *        or DLB_SOCKET_OK
                                    */
dlb_socket_open_datagram
    (dlb_socket* socket            /**< [out] datagram socket to create */
    ,char *iface                   /**< [in] addr of interface or NULL for default */
    ,int local_port                /**< [in]  */
    ,int* err
    );


/**
 * @brief write data to a stream socket
 */
dlb_socket_res                     /** @return DLB_SOCKET_INVALID_ARG
                                    *          DLB_SOCKET_LIBRARY_ERR
                                    *        or DLB_SOCKET_OK
                                    */
dlb_socket_stream_write
   (dlb_socket socket             /**< [in] socket to write to */
   ,void* buff                    /**< [in] data to write */
   ,size_t send_len               /**< [in] size of data in bytes */
   ,int* err                      /**< [out] system-level error code */
   );


/**
 * @brief write data to a datagram socket
 */
dlb_socket_res                     /** @return DLB_SOCKET_INVALID_ARG
                                    *          DLB_SOCKET_LIBRARY_ERR
                                    *        or DLB_SOCKET_OK
                                    */
dlb_socket_datagram_write
    (dlb_socket socket             /**< [in] socket to write to */
    ,char* other_address           /**< [in] address to send to */
    ,int other_port                /**< [in] target port */
    ,void* buff                    /**< [in] data to send */
    ,size_t send_len               /**< [in] length of data to send */
    ,int* err                      /**< [out] system-level error code */
    );


/**
 * @brief check socket validity
 */
int                                /** @return 1 if valid, 0 otherwise */
dlb_socket_valid
    (dlb_socket socket             /**< [in] socket to check */
    );


/**
 * @brief set the maximum socket receive buffer
 *
 * When receiving large amounts of UDP traffic, it may be
 * necessary to increase the buffer size to avoid dropping packets
 */
dlb_socket_res                     /** @return DLB_SOCKET_INVALID_ARG
                                    *          DLB_SOCKET_LIBRARY_ERR
                                    *       or DLB_SOCKET_OK
                                    */
dlb_socket_set_rcvbuf
    (dlb_socket socket             /**< [in] socket to use */
    ,size_t bytes                  /**< [in] receive buffer size in bytes */
    ,int* err                      /**< [out] system-level error code */
    );


/**
 * @brief Add a (UDP) socket to a multicast group
 *
 * _Multicasting_ is the ability to deliver a single stream of packets
 * from one server to multiple clients simultaneously.  Typical
 * applications might include streaming TV signals over the internet.
 * The alternative, where each client individually connects to the
 * server and requests a point-to-point download would flood the
 * internet and put undue strain on the server when the number of
 * clients grew large.
 *
 * Multicasting is a subscription event: clients have to add
 * themselves (and then detach) to particular multicast addresses.
 *
 * IPv4 multicast addresses are designated _class D_ addresses, where
 * the top-4 bits of the IP address are all 1.
 *
 * Once a socket joins a multicast group, the computer will start to
 * accept datagrams from that group until either all sockets
 * explicitly leave the group, or are closed.  Closing a socket
 * implicitly drops membership of the group.
 */
dlb_socket_res                     /** @return DLB_SOCKET_INVALID_ARG
                                    *          DLB_SOCKET_LIBRARY_ERR
                                    *       or DLB_SOCKET_OK
                                    */
dlb_socket_join_mcast_group
    (dlb_socket socket             /**< [in] socket to use */
    ,char* group_addr              /**< [in] multicast address to bind to */
    ,char* iface                   /**< [in] multicast interface addr or NULL for default */
    ,char* igmp_src_addr           /**< [in] IGMP v3 source addr or NULL */
    ,int* err                      /**< [out] system-level error code */
    );


/**
 * @brief Remove a (UDP) socket from a multicast group
 */
dlb_socket_res   /** @return DLB_SOCKET_INVALID_ARG
                  *          DLB_SOCKET_LIBRARY_ERR
                  *       or DLB_SOCKET_OK
                  */
dlb_socket_leave_mcast_group
    (dlb_socket socket             /**< [in] socket to use */
    ,char* group_addr              /**< [in] multicast address to leave */
    ,int* err                      /**< [out] system-level error code */
    );


/**
 * @brief block on activity over a number of sockets
 *
 * To improve efficiency, one shouldblock while waiting on a number
 * of different events, both read and write.
 *
 * This call will block waiting on activity on a read or write socket.
 * Note that one can specify a timeout to recover from stuck
 * situations.  However, this incurs more system overhead than simple
 * indefinite blocking.  If one wishes the ability to interrupt a
 * blocking socket (say at system shutdown) then the most portable way
 * is to create a socket pair, and include one end, the read end, in
 * the list of waiting sockets.
 */
dlb_socket_res              /** @return DLB_SOCKET_INVALID_ARG
                             *          DLB_SOCKET_NO_DATA if timed out
                             *          DLB_SOCKET_OK on success
                             */
dlb_socket_select
    (dlb_socket* readers    /**< [in] array of sockets to read */
    ,dlb_socket* writers    /**< [in] array of sockets to write */
    ,int num_readers        /**< [in] number of read sockets */
    ,int num_writers        /**< [in] number of write sockets */
    ,int timeout_msec       /**< [in] timeout or DLB_SOCKET_WAIT_FOREVER */
    ,int* num_sockets       /**< [out] number of sockets ready */
    ,int* err               /**< [out] system-level error code */
    );


/**
 * @brief read from stream (TCP) or datagram (UDP) socket
 */
dlb_socket_res 
dlb_socket_read
    (dlb_socket socket        /**< [in] socket to read from */
    ,void* buff               /**< [in] target buffer */
    ,size_t buff_len          /**< [in] capacity of target buffer */
    ,ssize_t *res_len         /**< [out] size of data actually read */
    ,dlb_socket_mode mode     /**< [in] blocking or non-blocking */
    ,int* err                 /**< [out] system-level error code */
    );


/**
 * @brief tidy up system resources
 */
dlb_socket_res               /** @return DLB_SOCKET_INVALID_ARG
                              *          DLB_SOCKET_OK
                              */
dlb_socket_close
    (dlb_socket socket        /**< [in] socket to close */
    );


/**
 * @brief create a socket pair
 *
 * A socket pair is a collection of two sockets that come
 * ready-connected to each other.  One use is to wake up an infinitely
 * blocking select call cheaply: just add one end to the array of
 * reader sockets, and simply write to the other end when one wishes
 * to wake it up.
 *
 * @note all socket pairs are created in domain AF_INET, and the
 * default protocol will be used for the given type
 *
 * @note both sockets must be closed individually via dlb_socket_close
 */
dlb_socket_res /** @return DLB_SOCKET_INVALID_ARG if arguments are wrong
                *          DLB_SOCKET_LIBRARY_ERR if system could not create
                *          DLB_SOCKET_OK          on success 
                */
dlb_socket_pair
    (int datagram          /**< [in] 1 if datagram socket, 0 if stream socket */
    ,dlb_socket *p_socket1 /**< [out] first socket of pair */
    ,dlb_socket *p_socket2 /**< [out] second socket of pair */
    ,int *err              /**< [out] system-level error code */
    );


#endif /* dlb_socket_H */
