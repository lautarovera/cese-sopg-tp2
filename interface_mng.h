/**
 * @file interface_mng.h
 * @author Lautaro Vera (lautarovera93@gmail.com)
 * @brief API declaration of Interface Manager
 * @version 0.1
 * @date 2022-06-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef INTERFACE_MNG_H
#define INTERFACE_MNG_H

/**
 * @brief Open and configures a socket as a server for the IP 127.0.0.1 and port 10000
 * 
 * @return int retcode
 * @see interface_print_error
 */
int interface_open(void);

/**
 * @brief Send a data buffer to the socket client connected with interface_open
 * 
 * @param data buffer for sending data
 * @param size size of the buffer
 * @return int number of bytes sent, negative number indicates an error
 */
int interface_send(char *data, int size);

/**
 * @brief Receive a data buffer from the socket client connected with interface_open
 * 
 * @param buf buffer to retrieve the data
 * @param size size of the buffer
 * @return int number of bytes received, negative number indicates an error
 */
int interface_receive(char *buf, int size);

/**
 * @brief Close the server socket
 * 
 */
void interface_close(void);

/**
 * @brief Print a verbosity of interface_open's return code
 * 
 * @param retcode return code of interface_open
 */
void interface_print_error(int retcode);

#endif