#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>		/* superset of previous */
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "mysql/mysql.h"
#include <stdbool.h>
#include <arpa/inet.h>

void serchData();
char *bin2hex(const unsigned char *input, size_t len);
bool QueryAnalyzer(unsigned char []);

static char *host = "localhost";
static char *user = "root";
static char *pw = "Speck3220#";
static char *proxyDB = "proxy";
unsigned int port = 3306;
static char *unix_socket = NULL;
unsigned int flag = 0;

#endif