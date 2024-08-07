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

/**
 * @file test_server.c
 * @brief very simple web server
 *
 * A very trivial web server that only handles one connection at a
 * time and only serves up the current directory listing.  It doesn't
 * actually parse requests.
 */


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "dlb_socket/include/dlb_socket.h"


#define PORT 22221
#define BUFLEN 20000
#define LINESIZE 1024


#if defined (_linux) || defined(__CYGWIN__)

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

struct dirabs
{
    DIR *d;
    char name[sizeof(((struct dirent *)0)->d_name)];
    int isdir;
    char datetime[64];
    int filesize;
};



void dirabs_next (struct dirabs *dir)
{
    struct dirent *dirent;

    if (NULL != dir->d)
    {
        dirent = readdir(dir->d);
        if (NULL != dirent)
        {
            struct stat st;
            int r = stat(dirent->d_name, &st);
            if (0 != r)
            {
                dirabs_next(dir);
            }
            else
            {
                strncpy(dir->name, dirent->d_name, sizeof(dir->name));
                dir->isdir = S_ISDIR(st.st_mode);
                if (dir->isdir)
                {
                    dir->filesize = 0;
                }
                else
                {
                    dir->filesize = st.st_size;
                }
                strftime(dir->datetime, sizeof(dir->datetime),
                         "%Y-%m-%d %H:%M",
                         localtime(&st.st_mtime));
            }
            return;
        }
    }
    dir->name[0] = '\0';
    dir->isdir = 0;
    dir->datetime[0] = '\0';
    dir->filesize = 0;
}




void dirabs_open (struct dirabs *dir)
{
    dir->d = opendir(".");
    if (NULL == dir->d)
    {
        dir->name[0] = '\0';
        dir->isdir = 0;
        dir->datetime[0] = '\0';
        dir->filesize = 0;
    }
    else
    {
        dirabs_next(dir);
    }
}


void dirabs_close (struct dirabs *dir)
{
    closedir(dir->d);
    dir->d = NULL;
    dir->name[0] = '\0';
    dir->isdir = 0;
    dir->datetime[0] = '\0';
    dir->filesize = 0;
}

            

#elif defined(_MSC_VER)

#include <windows.h>
#pragma comment(lib, "User32.lib")

struct dirabs
{
    WIN32_FIND_DATA ffd;
    char name[MAX_PATH];
    HANDLE hFind;

    int isdir;
    char datetime[64];
    LONGLONG filesize;
};



void dirabs_next (struct dirabs *dir)
{
    if (INVALID_HANDLE_VALUE != dir->hFind)
    {
        LARGE_INTEGER filesize;
        SYSTEMTIME stUtc;
        SYSTEMTIME stLocal;

        dir->isdir = 0;
        if (dir->ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            dir->isdir = 1;
        }

        strncpy(dir->name, dir->ffd.cFileName, sizeof(dir->name));
        filesize.LowPart = dir->ffd.nFileSizeLow;
        filesize.HighPart = dir->ffd.nFileSizeHigh;
        dir->filesize = filesize.QuadPart;

        FileTimeToSystemTime(&dir->ffd.ftLastWriteTime, &stUtc);
        SystemTimeToTzSpecificLocalTime(NULL, &stUtc, &stLocal);
        
        _snprintf(dir->datetime, sizeof(dir->datetime),
                 "%04d-%02d-%02d %02d:%02d",
                 stLocal.wYear, stLocal.wMonth, stLocal.wDay,
                 stLocal.wHour, stLocal.wMinute);
        if (0 == FindNextFileA(dir->hFind, &dir->ffd))
        {
            /* failure: no more files */
            FindClose(dir->hFind);
            dir->hFind = INVALID_HANDLE_VALUE;
        }
        return;
    }
    dir->name[0] = '\0';
    dir->isdir = 0;
    dir->datetime[0] = '\0';
    dir->filesize = 0;
}




void dirabs_open (struct dirabs *dir)
{
    _snprintf(dir->name, sizeof(dir->name), "*");

    dir->hFind = FindFirstFileA(dir->name, &dir->ffd);
    if (INVALID_HANDLE_VALUE == dir->hFind)
    {
        dir->name[0] = '\0';
        dir->isdir = 0;
        dir->datetime[0] = '\0';
        dir->filesize = 0;
    }
    else
    {
        dirabs_next(dir);
    }
}


void dirabs_close (struct dirabs *dir)
{
    if (INVALID_HANDLE_VALUE != dir->hFind)
    {
        FindClose(dir->hFind);
        dir->hFind = INVALID_HANDLE_VALUE;
    }
    dir->name[0] = '\0';
    dir->isdir = 0;
    dir->datetime[0] = '\0';
    dir->filesize = 0;
}


#define snprintf _snprintf


#else 

#error cant build for this OS

#endif








static
int
parse_http_request
     (dlb_socket socket
     )
{
    dlb_socket_res res;
    char buf[LINESIZE];
    char method[LINESIZE];
    char uri[LINESIZE];
    size_t len;
    int err;

    res = dlb_socket_read(socket, buf, sizeof(buf), &len, DLB_SOCKET_BLOCKING, &err);
    if (DLB_SOCKET_OK == res)
    {
        /* extract method and uri from request */
        char *end = strstr(buf, "\r\n\r\n");
        if (NULL == end)
        {
            printf("header too long\n");
            return 0;
        }
    
        sscanf(buf, "%s %s", method, uri);
        printf("Method: %s\n", method);
        printf("URI: %s\n", uri);
        return 1;
    }
    return 0;
}




static
void
process_connection
     (dlb_socket socket
     ,char *client
     )
{
    dlb_socket_res res;
    char buf[BUFLEN];
    int err;
    
    printf("Accepted connection from client %s\n", client);

    if (parse_http_request(socket))
    {
        int len;
        int size = BUFLEN;
        char *wp = buf;
        struct dirabs dir;
        
        len = snprintf(wp, size,
                       "HTTP/1.1 200 OK\r\n"
                       "Server: Dolby dlb_socket test server\r\n"
                       "Content-Type: text/html\r\n\r\n"
                       "<html><head><style>body{font-family: monospace; font-size: 15px;}"
                       "td {padding: 2px 8px;}"
                       "</style></head><body>"
                       "Dolby dlb_socket test_server\n"
                       "<table>\n");
        wp += len;
        size -= len;

        dirabs_open(&dir);
        while ('\0' != dir.name[0] && size > 0)
        {
            if (0 != strcmp(dir.name, ".") && 0 != strcmp(dir.name, ".."))
            {
                len = snprintf(wp, size,
                               "<tr><td>%s%s</td><td>%s</td><td>%d</td></tr>\n",
                               dir.name,
                               dir.isdir ? "/" : "",
                               dir.datetime,
                               dir.filesize);
                wp += len;
                size -= len;
            }
            dirabs_next(&dir);
        }
        dirabs_close(&dir);
        
        len = snprintf(wp, size, "</table></body></html>");
        size -= len;
        res = dlb_socket_stream_write(socket, buf, BUFLEN - size, &err);
        if (DLB_SOCKET_OK != res)
        {
            printf("ERROR sending response: %d\n", err);
        }
    }
}



int main (int argc, const char **argv)
{
    int err;
    dlb_socket socket;
    dlb_socket socket_client;
    dlb_socket_res res;
    char client[DLB_SOCKET_MAX_ADDRESS_SIZE];
    
    printf("trivial sample webserver: point browser to localhost:%d\n", PORT);

    dlb_socket_create_stream_server(&socket, PORT, &err);
    
    while (1)
    {
        res = dlb_socket_accept(socket, &socket_client, DLB_SOCKET_BLOCKING, &client, &err);
        if (DLB_SOCKET_OK == res)
        {
            process_connection(socket_client, client);
            dlb_socket_close(socket_client);
        }
        else
        {
            printf("socket accept error %d\n", err);
        }
    }
}
