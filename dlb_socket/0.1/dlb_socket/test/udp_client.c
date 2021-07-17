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

/** @file dolby_socket_test.c
 *  @brief Thin wrapper on system dependent sockets - testing application
 */


#include <stdio.h>
#include "dlb_socket/include/dlb_socket.h"

#define BUFLEN 20000
#define PORT 22221





int main()
{
    char buf[BUFLEN];
    dlb_socket socket;
    size_t read_len;
    
    dlb_socket_open_datagram(&socket, PORT);
    
    while(1)
    {
        dlb_socket_read(socket, buf, BUFLEN, &read_len, 0);
        printf("Read packet size:%d\n", read_len);
        
        dlb_socket_write(socket, "127.0.0.1", 22222, buf, 20);
        printf("Sent packet size:%d\n", 20);
        
        sleep(1);
    }
    
    dlb_close_socket(&socket);
}
