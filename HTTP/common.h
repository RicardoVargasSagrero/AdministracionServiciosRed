/*The folling code it's provided by https://www.youtube.com/watch?v=esXw4bdaZkc*/
#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/socket.h> /*basic socket definitions*/
#include <sys/types.h> 
#include <signal.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h> /* for inet_pton, an the like*/
#include <stdarg.h> /* for variable argument functions, like err_n_die*/
#include <errno.h> 
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
//useful constants 
#define SERVER_PORT 80

#define MAXLINE 4096 //why? 
#define SA struct sockaddr

void err_n_die(const char *fmt, ...);
char *bin2hex(const unsigned char *input, size_t len);
void setHttpHeader(char httpHeader[]);

#endif