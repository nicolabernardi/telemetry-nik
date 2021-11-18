#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 80

int socket_init(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if(sockfd == -1){
        fprintf(stderr, "Error on creating socket\n");
    }
    printf("sockfd = %d\n", sockfd);

    int result = connect(sockfd, (struct sockaddr*)&server, sizeof(server));
    if(result < 0){
        fprintf(stderr, "Error on binding the socket\n");
    }
    printf("socket has been bindED\n");

    return result < 0 ? -1 : sockfd;
}

int socket_send(int sockfd, char *buffer, int size){
    int result = send(sockfd, buffer, size, 0);
    if(result < 0){
        fprintf(stderr, "Error on sending data to the server\n");
    }
    printf("sent to the server: %s\n", buffer);
    return result;
}

char* socket_rcv(int sockfd, int size){
    char* buffer = (char*) malloc(sizeof(char)*(size+1));
    buffer[0] = 0; //metto una stringa vuota inizialmente
    int result = recv(sockfd, buffer, 2000, 0);
    if(result < 0){
        fprintf(stderr, "Error while trying to receive data from server\n");
    }
    printf("received from the server: %s\n", buffer);
    return buffer;
}


int main(){
    
    int sockfd = socket_init();
    char *message = "la mia stringa di pr0fafasodkaosva";
    socket_send(sockfd, message, strlen(message));
    socket_rcv(sockfd, 2000);
    close(sockfd);

    return 0;
}