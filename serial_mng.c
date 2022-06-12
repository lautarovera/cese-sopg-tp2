/**
 * @file SerialManager.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include "serial_mng.h"

static int serial_socket;

int serial_open(void)
{
    struct sockaddr_in serveraddr;
    int flags;
    int ret_val = 0;

    serial_socket = socket(PF_INET, SOCK_STREAM, 0);
    flags = fcntl(serial_socket, F_GETFL);
    fcntl(serial_socket, F_SETFL, flags | O_NONBLOCK);
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(4040);
    if (0 >= inet_pton(AF_INET, "127.0.0.1", &(serveraddr.sin_addr)))
    {
        fprintf(stderr, "ERROR invalid server IP\r\n");
        ret_val = -1;
    }

    if (0 == ret_val)
    {
        for (;;)
        {
            printf("Conectando a emulador...\n");
            int retcode = connect(serial_socket, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));
            printf("Connect response:%d\n", retcode);
            if (0 <= retcode)
            {
                usleep(100000);
                break;
            }
            sleep(1);
        }
        printf("Emulador conectado\n");

        ret_val = 0;
    }

    return ret_val;
}

int serial_send(char *data, int size)
{
    return write(serial_socket, data, size);
}

int serial_receive(char *buf, int size)
{
    return read(serial_socket, buf, size);
}

void serial_close(void)
{
    close(serial_socket);
}
