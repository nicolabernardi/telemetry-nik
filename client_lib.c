#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

// passing address (string) and port (int) it will return a socket as FD
int connectToServer(char* address, int port);
int sendMessage(char message[], int size, int sockfd);

int connectToServer(char* address, int port) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Create a socket point
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0) {
       perror("ERROR opening socket");
       return -1;
    }

    server = gethostbyname(address);

    if(server == NULL) {
       fprintf(stderr, "ERROR, no such host\n");
       return -2;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server -> h_addr,
          (char *) &serv_addr.sin_addr.s_addr,
          server -> h_length);
    serv_addr.sin_port = htons(port);

    // Now connect to the server
    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
       perror("ERROR connecting");
       return -3;
    }

    return sockfd;
}

// take the message and its size and send it to the server on the given socket
int sendMessage(char message[], int size, int sockfd) {
    int feedback;

    feedback = write(sockfd, message, strlen(message));

    if(feedback < 0) {
        perror("ERROR writing to socket");
        return -1;
    }

    bzero(message, size);
    /*feedback = read(sockfd, message, size - 1);

    if(feedback < 0) {
        perror("ERROR reading from socket");
        return -2;
    }*/

    printf("%s\n", message);
    return 0;
}