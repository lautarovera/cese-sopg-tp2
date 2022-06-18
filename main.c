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
static bool serial_enabled = false;
static bool interface_enabled = false;
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
 * @brief Initializes the handling of the signals SIGINT and SIGTERM.
 *
 */
static void sig_init(void)
{
    struct sigaction sigint, sigterm;

    sigint.sa_handler = sig_handler;
    sigint.sa_flags = SA_RESTART;
    sigterm.sa_handler = sig_handler;
    sigterm.sa_flags = SA_RESTART;

    sigemptyset(&sigint.sa_mask);
    if (sigaction(SIGINT, &sigint, NULL) == -1)
    {
        perror("sigaction on SIGINT");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&sigterm.sa_mask);
    if (sigaction(SIGTERM, &sigint, NULL) == -1)
    {
        perror("sigaction on SIGTERM");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Task which manages the reception from serial and the further sending to the interface.
 *
 */
static void serial_task(void)
{
    char buffer_serial[SERIAL_BUFFER_SIZE];
    int retcode, n_bytes;

    retcode = serial_open();
    serial_enabled = (0 <= retcode) ? true : false;

    while (serial_enabled & !done)
    {
        n_bytes = serial_receive(buffer_serial, SERIAL_BUFFER_SIZE);
        if (0 < n_bytes)
        {
            int output, state;

            if (2 == sscanf(buffer_serial, SERIAL_SWITCH_EVENT, &output, &state))
            {
                if ((0 <= output && 2 >= output) && (0 == state || 1 == state))
                {
                    // printf("Sending to interface: %s", buffer_serial);
                    n_bytes = interface_send(buffer_serial, strlen(buffer_serial));
                    if (-1 == n_bytes)
                    {
                        perror("Interface: write");
                    }
                }
                else
                {
                    fprintf(stderr, "Interface: invalid output or state\r\n");
                }
            }
            else
            {
                fprintf(stderr, "Interface: invalid serial frame\r\n");
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
    int retcode, n_bytes;

    retcode = interface_open();
    interface_print_error(retcode);
    interface_enabled = (0 == retcode) ? true : false;

    while (interface_enabled)
    {
        n_bytes = interface_receive(buffer_interface, INTERFACE_BUFFER_SIZE);
        if (0 < n_bytes)
        {
            int output, state;

            if (2 == sscanf(buffer_interface, INTERFACE_SET_EVENT, &output, &state))
            {
                if ((0 <= output && 2 >= output) && (0 == state || 1 == state))
                {
                    // printf("Sending to serial: %s", buffer_interface);
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

/*------------------------------------- Main -----------------------------------------------------*/
int main(void)
{
    pthread_t interface_thread;
    int retcode;

    printf("Inicio Serial Service\r\n");

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
