/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2013 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/** @file dlb_socket_os_impl.h
 *  @brief windows variant of socket layer
 */

#ifndef dlb_socket_os_impl_H
#define dlb_socket_os_impl_H

#ifdef  _WIN64
typedef __int64  ssize_t;
#else
typedef _W64 int ssize_t;
#endif

#include <winsock2.h>
#include <Ws2tcpip.h>
typedef SOCKET dlb_socket_os_impl;

#endif /* dlb_socket_os_impl_H */
