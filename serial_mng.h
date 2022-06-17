/**
 * @file serial_mng.h
 * @author Lautaro Vera (lautarovera93@gmail.com)
 * @brief API declaration of Serial Manager
 * @version 0.1
 * @date 2022-06-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef SERIAL_MNG_H
#define SERIAL_MNG_H

/**
 * @brief Open a client socket to connect the serial emulator server with IP 127.0.0.1 and port 4040
 * 
 * @return int 
 */
int serial_open(void);

/**
 * @brief Send a data buffer to the serial emulator server
 * 
 * @param data buffer for sending data
 * @param size size of the buffer
 * @return int number of bytes sent, negative number indicates an error
 */
int serial_send(char* data,int size);

/**
 * @brief Receive a data buffer from the serial emulator server
 * 
 * @param buf buffer to retrieve the data
 * @param size size of the buffer
 * @return int number of bytes received, negative number indicates an error
 */
int serial_receive(char* buf,int size);

/**
 * @brief Close the client socket
 * 
 */
void serial_close(void);

#endif
