#ifndef MINI_SERV
#define MINI_SERV
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <cstdio>
#include <cerrno>
#include <arpa/inet.h>
#include <sys/socket.h>



void write_error_message(int errnum);

#endif