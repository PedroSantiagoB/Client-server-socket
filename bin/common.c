#include "common.h"

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <arpa/inet.h>

// log an error message and exit the program
void logexit(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

// parse IP address and port number into sockaddr_storage structure
int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage)
{
    if (addrstr == NULL || portstr == NULL)
    {
        return -1;
    }

    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0)
    {
        return -1;
    }

    port = htons(port); // host to network short

    struct in_addr inaddr4;                    // 32-bit IPv4 address
    if (inet_pton(AF_INET, addrstr, &inaddr4)) // convert IPv4 string to binary
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET; // set address family to IPv4
        addr4->sin_port = port;      // set port
        addr4->sin_addr = inaddr4;   // set IPv4 address
        return 0;                    // return success
    }

    struct in6_addr inaddr6;                    // 128-bit IPv6 address
    if (inet_pton(AF_INET6, addrstr, &inaddr6)) // convert IPv6 string to binary
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;                          // set address family to IPv6
        addr6->sin6_port = port;                                // set port
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6)); // set IPv6 address
        return 0;                                               // return success
    }
    return -1;
}

// initialize server address information based on protocol and port number
int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage)
{
    uint16_t port = (uint16_t)atoi(portstr); // unsigned short
    if (port == 0)
    {
        return -1;
    }
    port = htons(port); // host to network short

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4"))
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;         // set address family to IPv4
        addr4->sin_addr.s_addr = INADDR_ANY; // bind to any available IPv4 address
        addr4->sin_port = port;              // set port
        return 0;                            // return success
    }
    else if (0 == strcmp(proto, "v6"))
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;  // set address family to IPv6
        addr6->sin6_addr = in6addr_any; // bind to any available IPv6 address
        addr6->sin6_port = port;        // set port
        return 0;                       // return success
    }
    else
    {
        return -1; // return -1 if protocol is neither "v4" nor "v6"
    }
}