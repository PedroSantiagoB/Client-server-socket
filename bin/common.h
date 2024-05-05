#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

void logexit(const char *str);

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage);