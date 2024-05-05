#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

typedef struct
{
    double latitude;
    double longitude;
} Coordinate;

void printMenu()
{
    printf("0. Sair\n1. Solicitar Corrida\n");
}

void displayMessage(char *message)
{
    printf("%s\n", message);
}

void usage(int argc, char **argv)
{
    printf("Usage: %s <serverIP> <server port>", argv[0]);
    printf("example: %s 127.0.0.1 51511", argv[0]);
    exit(EXIT_FAILURE);
}

#define BUFSZ 1024

int main(int argc, char **argv)
{

    // check if the correct number of command-line arguments are provided
    if (argc < 3)
    {
        usage(argc, argv);
    }
    // initialize a sockaddr_storage struct to hold server address information
    struct sockaddr_storage storage;

    // initialize server address information based on command-line arguments
    if (0 != addrparse(argv[1], argv[2], &storage))
    {
        usage(argc, argv);
    }

    Coordinate clientCoord = {-19.979118818922608, -43.977958346680076};

    // variable to hold user's choice for ride request, start with invalid value
    int ride = -1;

    // loop to interact with the user until the user chooses to exit
    while (ride != 0)
    {
        printMenu();
        scanf("%d", &ride);

        if (ride == 0)
        {
            break;
        }
        else if (ride == 1)
        {
            // create a socket for the client to communicate with the server
            int s;
            s = socket(storage.ss_family, SOCK_STREAM, 0);
            if (s == -1)
            {
                logexit("socket");
            }

            // connect the client socket to the server
            struct sockaddr *addr = (struct sockaddr *)&storage;
            if (connect(s, addr, sizeof(storage)) != 0)
            {
                logexit("connect");
            }

            // Sending client coordinates to the server
            if (send(s, &clientCoord, sizeof(Coordinate), 0) == -1)
            {
                logexit("send");
            }

            // Receiving and displaying response from the server in real-time
            char buf[BUFSZ];
            int count;
            while ((count = recv(s, buf, BUFSZ, 0)) > 0)
            {
                buf[count] = '\0'; // Null-terminate the received data
                displayMessage(buf);
            }
            close(s);
            if (strcmp(buf, "Não foi encontrado um motorista."))
            {
                break; // Exiting after receiving response
            }
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
        }
    }
    exit(EXIT_SUCCESS);
}
