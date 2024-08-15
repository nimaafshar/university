#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>


#define BUFFER_SIZE 8192

void uppercase_string(char *string, int size) {
    for (int i = 0; i < size; i++) {
        string[i] = toupper(string[i]);
    }
}

int main(int argc, char *argv[]) {
    int server_fd;//initial server descriptor
    int connected_fd; //connected socket descriptor
    int socket_opt = 1;
    struct sockaddr_in socket_address;
    char *listenaddress = "127.0.0.1";
    int PORT = 5000; //portnumber

    //assigning listen address and port number from program arguments
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            listenaddress = argv[i + 1];
        } else if (strcmp(argv[i], "-p") == 0) {
            PORT = atoi(argv[i + 1]);
        }
    }

    //creating the socket FD
    if ((server_fd = socket(AF_INET/*IPV4*/, SOCK_STREAM/*TCP*/, 0/*ip protocol*/)) == 0) {
        perror("creating socket failed");
        exit(EXIT_FAILURE);
    }

    //prevent reuse of address and port
    if (setsockopt(server_fd, SOL_SOCKET, (SO_REUSEADDR | SO_REUSEPORT), &socket_opt, sizeof(socket_opt)) < 0) {
        perror("setting option failed");
        exit(EXIT_FAILURE);
    }

    //bind socket to ip and port
    socket_address.sin_family = AF_INET;//ipv4
//    socket_address.sin_addr.s_addr = INADDR_ANY; //any address on the machine
    inet_pton(AF_INET, listenaddress, &socket_address.sin_addr);
    socket_address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *) &socket_address, sizeof(socket_address)) < 0) {
        perror("binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 2) == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("\nserver: listening on port %d", PORT);
    int connection_count = 0;
    for (;;) {

        //set the size variable that will be referred in the connection
        int address_size = sizeof(socket_address);
        if ((connected_fd = accept(server_fd, (struct sockaddr *) &socket_address, (socklen_t *) &address_size)) < 0) {
            perror("connection could not be accepted");
            exit(EXIT_FAILURE);
        }
        //prevent reuse of address and port
        if (setsockopt(connected_fd, SOL_SOCKET, (SO_REUSEADDR | SO_REUSEPORT), &socket_opt, sizeof(socket_opt)) < 0) {
            perror("setting option failed");
            exit(EXIT_FAILURE);
        }
        ++connection_count;
        printf("\nserver: new connection,number=%d", connection_count);
        int fork_result = fork();
        if (fork_result < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (fork_result == 0) {
            //we are in child process
            int pid = getpid();
            printf("\nserver: fork with pid:%d", pid);
            char buffer[BUFFER_SIZE] = {0};
            int reading_status = read(connected_fd, buffer, BUFFER_SIZE);
            if (reading_status == -1) {
                perror("error in reading from connection");
                exit(EXIT_FAILURE);
            }
            printf("\nserver:\"%s\" read from client %d", buffer, connection_count);
            uppercase_string(buffer, strlen(buffer));
            send(connected_fd, buffer, BUFFER_SIZE, 0);
            printf("\nserver: \"%s\" sent to client %d", buffer, connection_count);
            shutdown(connected_fd, SHUT_WR);
            close(connected_fd);
            exit(EXIT_SUCCESS);
        } else {
            //we are in parent process
            //so we close the connection in this process
            close(connected_fd);
        }
    }
    return 0;
}