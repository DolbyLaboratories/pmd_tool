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
