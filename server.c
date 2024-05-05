#include "common.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#define BUFSZ 1024

#ifndef M_PI // defining pi
#define M_PI 3.14159265358979323846
#endif

typedef struct
{
    double latitude;
    double longitude;
} Coordinate;

double calculateDistance(Coordinate coord1, Coordinate coord2)
{
    // converting degrees to radians
    double lat1Rad = coord1.latitude * M_PI / 180.0;
    double lon1Rad = coord1.longitude * M_PI / 180.0;
    double lat2Rad = coord2.latitude * M_PI / 180.0;
    double lon2Rad = coord2.longitude * M_PI / 180.0;

    // diff lat and lon
    double dLat = lat2Rad - lat1Rad;
    double dLon = lon2Rad - lon1Rad;

    // Haversine formula
    double a = pow(sin(dLat / 2), 2) + cos(lat1Rad) * cos(lat2Rad) * pow(sin(dLon / 2), 2);
    double c = 2 * asin(sqrt(a));
    double rad = 6371; // radius of Earth in kilometers
    double distance = rad * c;

    return distance * 1000; // meters
}

void printWaitingMessage()
{
    printf("Aguardando solicitação.\n");
}

void printConnectionMessage()
{
    printf("Corrida disponível: \n");
    printf("0 - Recusar\n1 - Aceitar\n");
}

void sendDistanceMessage(int clientSocket, double distance)
{
    char message[BUFSZ];
    if (distance > 0)
        sprintf(message, "Motorista a %.0fm", distance);
    else
        strcpy(message, "O motorista chegou.");
    send(clientSocket, message, strlen(message), 0);
}

void usage(int argc, char **argv)
{
    printf("Usage: %s <v4|v6> <server port>", argv[0]);
    printf("example: %s v4 51511", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    // check if the correct number of arguments are provided
    if (argc < 3)
    {
        usage(argc, argv);
    }

    // initialize a sockaddr_storage struct to hold server address information
    struct sockaddr_storage storage;

    // initialize server address information based on command-line arguments
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage))
    {
        usage(argc, argv);
    }

    // create a socket for the server to listen for incoming connections
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    {
        logexit("socket");
    }

    // enable reuse of address and port to avoid "Address already in use" errors
    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)))
    {
        logexit("setsockopt");
    }

    // bind the socket to the server address
    struct sockaddr *addr = (struct sockaddr *)&storage;
    if (0 != bind(s, addr, sizeof(storage)))
    {
        logexit("bind");
    }

    // start listening for incoming connections on the bound socket
    if (0 != listen(s, 10))
    {
        logexit("listen");
    }

    while (1)
    {
        // waiting connection from client
        printWaitingMessage();
        struct sockaddr_storage clientStorage;
        struct sockaddr *clientAddr = (struct sockaddr *)&clientStorage;
        socklen_t clientAddrLen = sizeof(clientStorage);
        int clientSocket = accept(s, clientAddr, &clientAddrLen);
        if (clientSocket == -1)
        {
            logexit("accept");
        }

        // accept or refuse ride
        printConnectionMessage();
        int takeRide;
        scanf("%d", &takeRide);

        if (takeRide == 1)
        {
            Coordinate driverCoord = {-19.9227, -43.9451};
            Coordinate clientCoord;

            //  receiving client coordinates
            if (recv(clientSocket, &clientCoord, sizeof(Coordinate), 0) == -1)
            {
                logexit("recv");
            }

            double distance = calculateDistance(driverCoord, clientCoord);

            while (distance > 0)
            {
                sendDistanceMessage(clientSocket, distance);
                usleep(2000000); // 2 secs
                distance -= 400; // 400 meters
            }

            sendDistanceMessage(clientSocket, 0);

            // closing client connection
            close(clientSocket);
        }
        else if (takeRide == 0)
        { // refuse ride
            char refusalMessage[] = "Não foi encontrado um motorista.";
            send(clientSocket, refusalMessage, strlen(refusalMessage), 0);

            // closing client connection
            close(clientSocket);
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
        }
    }

    // closing server socket
    close(s);
}