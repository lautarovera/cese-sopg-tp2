/**
 * @file interface_mng.c
 * @author Lautaro Vera (lautarovera93@gmail.com)
 * @brief Manages the connection with Interface Service component
 * @version 0.1
 * @date 2022-06-11
 *
 * @copyright Copyright (c) 2022
 *
 */
/*------------------------------------- Includes -------------------------------------------------*/
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
#include "interface_mng.h"

/*------------------------------------- Globals --------------------------------------------------*/
static int interface_socket;
static int interface_fd;

/*------------------------------------- Publics --------------------------------------------------*/
int interface_open(void)
{
    socklen_t addr_len;
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    char ip_client[32];
    int ret_val = 0;

    // Creamos socket
    interface_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Cargamos datos de IP:PORT del server
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(10000);

    if (inet_pton(AF_INET, "127.0.0.1", &(serveraddr.sin_addr)) <= 0)
    {
        ret_val = 1;
    }

    int reuse = 1;
    if (setsockopt(interface_socket, SOL_SOCKET, SO_REUSEADDR,
                   (const char *)&reuse, sizeof(reuse)) < 0)
    {
        perror("Interface setsockopt(SO_REUSEADDR)");
    }

    if (setsockopt(interface_socket, SOL_SOCKET, SO_REUSEPORT,
                   (const char *)&reuse, sizeof(reuse)) < 0)
    {
        perror("Interface setsockopt(SO_REUSEPORT)");
    }

    // Abrimos puerto con bind()
    if (bind(interface_socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        close(interface_socket);
        perror("Interface bind");
        ret_val = 2;
    }

    // Seteamos socket en modo Listening
    if (listen(interface_socket, 10) == -1) // backlog=10
    {
        perror("Interface listen");
        ret_val = 3;
    }
    else
    {
        while (1)
        {
            addr_len = sizeof(struct sockaddr_in);
            interface_fd = accept(interface_socket, (struct sockaddr *)&clientaddr, &addr_len);
            if (-1 == interface_fd)
            {
                perror("Interface accept");
                ret_val = -1;
            }
            else
            {
                inet_ntop(AF_INET, &(clientaddr.sin_addr), ip_client, sizeof(ip_client));
                printf("Interface: server connected from:  %s\n", ip_client);
                break;
            }
        }
    }

    return ret_val;
}

int interface_send(char *data, int size)
{
    return write(interface_fd, data, size);
}

int interface_receive(char *buf, int size)
{
    return read(interface_fd, buf, size);
}

void interface_close(void)
{
    close(interface_fd);
    printf("Interface: server disconnected");
}

void interface_print_error(int retcode)
{
    switch (retcode)
    {
    case 0:
        break;
    case 1:
        fprintf(stderr, "Interface error: invalid server IP\r\n");
        break;
    case 2:
        fprintf(stderr, "Interface error: fail to bind port\r\n");
        break;
    case 3:
        fprintf(stderr, "Interface error: fail to set socket to listen\r\n");
        break;
    case -1:
        fprintf(stderr, "Interface error: fail to accept connection\r\n");
        break;
    default:
        fprintf(stderr, "Interface error: unknown error\r\n");
        break;
    }
}