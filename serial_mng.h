/**
 * @file serial_mng.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef SERIAL_MNG_H
#define SERIAL_MNG_H

int serial_open(void);

int serial_send(char* data,int size);

int serial_receive(char* buf,int size);

void serial_close(void);

#endif
