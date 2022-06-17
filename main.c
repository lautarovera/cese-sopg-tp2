/**
 * @file main.c
 * @author Lautaro Vera (lautarovera93@gmail.com)
 * @brief Serial Service component
 * @version 0.1
 * @date 2022-06-11
 *
 * @copyright Copyright (c) 2022
 *
 */
/*------------------------------------- Includes -------------------------------------------------*/
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

/*------------------------------------- Defines --------------------------------------------------*/
#define SERIAL_BUFFER_SIZE 10
#define SERIAL_SWITCH_EVENT ">SW:%1d,%1d"
#define INTERFACE_BUFFER_SIZE 10
#define INTERFACE_SET_EVENT ">OUT:%1d,%1d"

/*------------------------------------- Globals --------------------------------------------------*/
static bool interface_ready = false;
static bool serial_ready = false;
static volatile sig_atomic_t done = 0;

/*------------------------------------- Privates -------------------------------------------------*/
/**
 * @brief Blocks the reception of the signals SIGINT and SIGTERM, ignoring them.
 * 
 */
static void lock_sign(void)
{
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
}

/**
 * @brief Unblocks the reception of the signals SIGINT and SIGTERM, allowing their handling.
 * 
 */
static void unlock_sign(void)
{
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    pthread_sigmask(SIG_UNBLOCK, &set, NULL);
}

/**
 * @brief Handler for both signals, SIGINT and SIGTERM.
 * 
 * @param sig not used, only to be compliant with the type definition of the handler.
 */
static void sig_handler(int sig)
{
    done = 1;
}

/**
 * @brief Task which manages the reception from serial and the further sending to the interface.
 * 
 */
static void serial_task(void)
{
    char buffer_serial[SERIAL_BUFFER_SIZE];
    int n_bytes;

    while (!done)
    {
        n_bytes = serial_receive(buffer_serial, SERIAL_BUFFER_SIZE);
        if (0 < n_bytes)
        {
            int output, state;

            if (2 == sscanf(buffer_serial, SERIAL_SWITCH_EVENT, &output, &state))
            {
                if ((0 <= output && 2 >= output) && (0 == state || 1 == state))
                {
                    printf("Sending to interface: %s", buffer_serial);
                    n_bytes = interface_send(buffer_serial, strlen(buffer_serial));
                    if (-1 == n_bytes)
                    {
                        perror("ERROR writing to interface");
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

/**
 * @brief Task to manages the reception from interface and the further sending to serial
 * 
 * @param args not used, only to be compliant with p_threads library.
 * @return void* not used, only to be compliant with p_threads library.
 */
static void *interface_task(void *args)
{
    char buffer_interface[INTERFACE_BUFFER_SIZE];
    int n_bytes;

    while (!done)
    {
        n_bytes = interface_receive(buffer_interface, INTERFACE_BUFFER_SIZE);
        if (0 < n_bytes)
        {
            int output, state;

            if (2 == sscanf(buffer_interface, INTERFACE_SET_EVENT, &output, &state))
            {
                if ((0 <= output && 2 >= output) && (0 == state || 1 == state))
                {
                    printf("Sending to serial: %s", buffer_interface);
                    n_bytes = serial_send(buffer_interface, strlen(buffer_interface));
                    if (-1 == n_bytes)
                    {
                        perror("Serial: write");
                    }
                }
                else
                {
                    fprintf(stderr, "Serial error: invalid output or state\r\n");
                }
            }
            else
            {
                fprintf(stderr, "Serial error: invalid interface frame\r\n");
            }
        }
    }

    interface_close();
}

/**
 * @brief Initializes the handling of the signals SIGINT and SIGTERM.
 * 
 */
static void sig_init(void)
{
    struct sigaction sig;

    sig.sa_handler = sig_handler;
    sig.sa_flags = 0;

    sigemptyset(&sig.sa_mask);
    if (sigaction(SIGINT, &sig, NULL) == -1)
    {
        perror("sigaction on SIGINT");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sig.sa_mask);
    if (sigaction(SIGTERM, &sig, NULL) == -1)
    {
        perror("sigaction on SIGTERM");
        exit(EXIT_FAILURE);
    }
}

/*------------------------------------- Main -----------------------------------------------------*/
int main(void)
{
    pthread_t interface_thread;
    int retcode;

    printf("Inicio Serial Service\r\n");

    retcode = serial_open();
    if (0 != retcode)
    {
        fprintf(stderr, "ERROR during serial open\r\n");
        exit(EXIT_FAILURE);
    }

    retcode = interface_open();
    if (0 != retcode)
    {
        interface_print_error(retcode);
        exit(EXIT_FAILURE);
    }

    sig_init();

    lock_sign();

    pthread_create(&interface_thread, NULL, interface_task, NULL);

    unlock_sign();

    serial_task();

    if (done)
    {
        pthread_cancel(interface_thread);
    }
    else
    {
        pthread_join(interface_thread, NULL);
    }

    exit(EXIT_SUCCESS);
    return 0;
}
