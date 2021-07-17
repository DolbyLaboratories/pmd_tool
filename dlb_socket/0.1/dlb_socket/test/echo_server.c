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

/** @file echo_server.c
 * @brief a simple echo server: returns what it is sent
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dlb_socket/include/dlb_socket.h"

#define BUFLEN 20000

int main()
{
    char buf[BUFLEN];
    int err;
    dlb_socket_res res;
    size_t res_len;
    dlb_socket socket;
    dlb_socket socket_client;

    printf("simple echo server test program.  run it, and then send\n"
           "data via netcat program:\n\n"
           "$ echo \"hello\" | nc localhost 2333\n\n"
           "and you should see 'hello' appear on the screen.  You can\n"
           "replace \"hello\" with whatever you like.\n\n"
           "To exit, simply send the string \"quit\"\n");


    dlb_socket_create_stream_server(&socket, 2333, &err);

    while (1)
    {
        dlb_socket_accept(socket, &socket_client, DLB_SOCKET_BLOCKING, NULL, &err);
        res = dlb_socket_read(socket_client, buf, BUFLEN, &res_len,
                              DLB_SOCKET_BLOCKING, &err);
        if (DLB_SOCKET_OK == res)
        {
            dlb_socket_stream_write(socket_client, buf, res_len, &err);
            if (0 == strncmp(buf, "quit", 4))
            {
                break;
            }
        }
        dlb_socket_close(socket_client);
    }
    dlb_socket_close(socket);
    return 0;
}
