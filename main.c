/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-06-11
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include "serial_mng.h"
#include "interface_mng.h"

#define SERIAL_BUFFER_SIZE 10
#define SERIAL_SWITCH_EVENT ">SW:%1d,%1d"

#define INTERFACE_BUFFER_SIZE 10
#define INTERFACE_SET_EVENT ">OUT:%1d,%1d"

static pthread_mutex_t mutex_serial = PTHREAD_MUTEX_INITIALIZER;

static bool interface_ready = false;
static bool serial_ready = false;

volatile sig_atomic_t done = 0;

static void lock_sign(void)
{
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
}

static void unlock_sign(void)
{
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_UNBLOCK, &set, NULL);
}

static void sigint_handler(int sig)
{
    done = 1;
}

static void sigterm_handler(int sig)
{
    done = 1;
}

static void *serial_task(void *args)
{
    char buffer_serial[SERIAL_BUFFER_SIZE];
    int retcode, n;

    retcode = serial_open();
    if (0 == retcode)
    {
        serial_ready = true;

        while (!done)
        {
            pthread_mutex_lock(&mutex_serial);
            n = serial_receive(buffer_serial, SERIAL_BUFFER_SIZE);
            pthread_mutex_unlock(&mutex_serial);

            if (0 < n)
            {
                int output, state;

                if (2 == sscanf(buffer_serial, SERIAL_SWITCH_EVENT, &output, &state))
                {
                    if ((0 >= output && 2 <= output) && (0 == state || 1 == state))
                    {
                        if (true == interface_ready)
                        {
                            n = interface_send(buffer_serial, strlen(buffer_serial));
                            if (-1 == n)
                            {
                                perror("ERROR writing to interface");
                            }
                        }
                        else
                        {
                            fprintf(stderr, "ERROR interface not ready\r\n");
                        }
                    }
                    else
                    {
                        fprintf(stderr, "ERROR invalid output or state\r\n");
                    }
                }
                else
                {
                    fprintf(stderr, "ERROR invalid serial frame\r\n");
                }
            }
        }

        serial_close();
    }
    else
    {
        fprintf(stderr, "ERROR during serial open\r\n");
    }
}

static void *interface_task(void *args)
{
    char buffer_interface[INTERFACE_BUFFER_SIZE];
    int retcode, n;

    retcode = interface_open();
    if (0 == retcode)
    {
        interface_ready = true;

        while (!done)
        {
            n = interface_receive(buffer_interface, INTERFACE_BUFFER_SIZE);
            if (0 < n)
            {
                int output, state;

                if (2 == sscanf(buffer_interface, INTERFACE_SET_EVENT, &output, &state))
                {
                    if ((0 >= output && 2 <= output) && (0 == state || 1 == state))
                    {
                        if (true == serial_ready)
                        {
                            n = serial_send(buffer_interface, strlen(buffer_interface));
                            if (-1 == n)
                            {
                                perror("ERROR writing to serial");
                            }
                        }
                        else
                        {
                            fprintf(stderr, "ERROR serial not ready\r\n");
                        }
                    }
                    else
                    {
                        fprintf(stderr, "ERROR invalid output or state\r\n");
                    }
                }
                else
                {
                    fprintf(stderr, "ERROR invalid interface frame\r\n");
                }
            }
        }

        interface_close();
    }
    else
    {
        fprintf(stderr, "ERROR during interface open\r\n");
    }
}

int main(void)
{
    struct sigaction sigint, sigterm;
    pthread_t serial_thread, interface_thread;
    int retcode;

    sigint.sa_handler = sigint_handler;
    sigint.sa_flags = 0;
    sigemptyset(&sigint.sa_mask);
    if (sigaction(SIGINT, &sigint, NULL) == -1)
    {
        perror("sigaction on SIGINT");
        exit(EXIT_FAILURE);
    }

    sigterm.sa_handler = sigterm_handler;
    sigterm.sa_flags = 0;
    sigemptyset(&sigterm.sa_mask);
    if (sigaction(SIGTERM, &sigterm, NULL) == -1)
    {
        perror("sigaction on SIGTERM");
        exit(EXIT_FAILURE);
    }

    printf("Inicio Serial Service\r\n");

    lock_sign();

    pthread_create(&serial_thread, NULL, serial_task, NULL);
    pthread_create(&interface_thread, NULL, interface_task, NULL);

    unlock_sign();

    pthread_join(serial_thread, NULL);
    pthread_join(interface_thread, NULL);

    exit(EXIT_SUCCESS);
    return 0;
}
