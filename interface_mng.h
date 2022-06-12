/**
 * @file interface_mng.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef INTERFACE_MNG_H
#define INTERFACE_MNG_H

int interface_open(void);

int interface_send(char *data, int size);

int interface_receive(char *buf, int size);

void interface_close(void);

#endif