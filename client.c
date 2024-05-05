#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

void usage(int argc, char **argv)
{
    printf("Usage: %s <serverIP> <server port>", argv[0]);
    printf("example: %s 127.0.0.1 51511", argv[0]);
    exit(EXIT_FAILURE);
}

#define BUFSZ 1024

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != addrparse(argv[1], argv[2], &storage))
    {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    {
        logexit("socket");
    }

    struct sockaddr *addr = (struct sockaddr *)&storage;
    if (connect(s, addr, sizeof(storage)) != 0)
    {
        logexit("connect");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    printf("connected to %s\n", addrstr);

    char buf[BUFSIZ];
    memset(buf, 0, BUFSIZ);
    printf("mensagem> ");
    fgets(buf, BUFSIZ - 1, stdin);
    // number of bytes in network
    int count = send(s, buf, strlen(buf) + 1, 0);
    if (count != strlen(buf) + 1)
    {
        logexit("send");
    }

    memset(buf, 0, BUFSIZ);
    unsigned total = 0;
    while (1)
    {
        count = recv(s, buf + total, BUFSIZ - total, 0);
        if (count == 0)
        {
            // connection terminated
            break;
        }
        total += count;
    }
    close(s);

    printf("received %d bytes\n", total);
    puts(buf);

    exit(EXIT_SUCCESS);
}