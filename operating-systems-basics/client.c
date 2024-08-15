//
// Created by Nima on 4/2/2020.
//
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#define BUFFER_SIZE 8192

int main(int argc, char *argv[]) {
    int socket_fd = 0;
    int PORT = 5000;
    char *server_ip = "127.0.0.1";
    char *message;
    struct timeval start, end;

    //assigning listen address and port number from program arguments
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            server_ip = argv[i + 1];
        } else if (strcmp(argv[i], "-p") == 0) {
            PORT = atoi(argv[i + 1]);
        }
    }
    message = argv[argc - 1];


    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("error while creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_port = htons(PORT);
    server_address.sin_family = AF_INET;
    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        perror("error getting address from string : invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE] = {0};

    gettimeofday(&start, NULL);
    //sending some message
    send(socket_fd, message, strlen(message), 0);

    int reading_status = read(socket_fd, buffer, BUFFER_SIZE);
    gettimeofday(&end, NULL);
    if (reading_status == -1) {
        perror("error in reading from connection");
        exit(EXIT_FAILURE);
    }

    printf("\nclient: \"%s\" received", buffer);
    printf("\nclient: took %f miliseconds\n",
           (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f);
    return 0;
}
