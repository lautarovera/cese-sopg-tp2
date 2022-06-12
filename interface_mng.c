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

static int interface_socket;
static int interface_fd;

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
    // serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (inet_pton(AF_INET, "127.0.0.1", &(serveraddr.sin_addr)) <= 0)
    {
        fprintf(stderr, "ERROR invalid server IP\r\n");
        ret_val = 1;
    }

    // Abrimos puerto con bind()
    if (bind(interface_socket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        close(interface_socket);
        perror("bind:");
        ret_val = 1;
    }

    // Seteamos socket en modo Listening
    if (listen(interface_socket, 10) == -1) // backlog=10
    {
        perror("listen:");
        ret_val = -1;
    }

    for (;;)
    {
        // Ejecutamos accept() para recibir conexiones entrantes
        addr_len = sizeof(struct sockaddr_in);
        if ((interface_fd = accept(interface_socket, (struct sockaddr *)&clientaddr, &addr_len)) == -1)
        {
            perror("accept:");
            ret_val = -1;
        }

        inet_ntop(AF_INET, &(clientaddr.sin_addr), ip_client, sizeof(ip_client));
        printf("server: conexion desde:  %s\n", ip_client);
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
}

