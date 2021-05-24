#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <stdbool.h>

#define CLIENT_NUMBER 1024

int getNewID(struct pollfd *connectionDescriptors){
    for (int i = 1; i < CLIENT_NUMBER; i++){
        if (connectionDescriptors[i].fd == -1){
            return i;
        }
    }
    return -1;
}

int main(int argc, char* argv[]){

    in_port_t destinationPort;

    if (argc == 2){
        destinationPort = (in_port_t)atoi(argv[1]);
    } else {
        printf("Incorrect arguments, server was initialized on default port 2000\n");
        destinationPort = 2000;
    }

    struct sockaddr_in ServerAddress;
    memset(&ServerAddress, 0 , sizeof(struct sockaddr_in));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(destinationPort);

    int sock;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return -1;
    }

    int FlagVal = 1;

    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &FlagVal, sizeof(int)) == -1)
    {
        perror("setsockopt");
        return -1;
    }

    /*присвоить имя дескриптору */
    if (bind(sock, (struct sockaddr *)&ServerAddress, sizeof(struct sockaddr_in)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    /* сообщить ядру, что процесс является сервером */
    if (listen(sock, 510) != 0){
        perror("listen");
        close(sock);
        return -1;
    }

    struct pollfd connectionDescriptors[CLIENT_NUMBER];

    // initialization
    for (int i = 0; i < CLIENT_NUMBER; i++){
        connectionDescriptors[i].fd = -1;
        connectionDescriptors[i].events = POLLIN;
        connectionDescriptors[i].revents = 0;
    }

    connectionDescriptors[0].fd = sock;
    int takenDescriptorsCount = 1; // one desc in taken for socket

    ssize_t readCount = 0;
    char buffer[BUFSIZ];
    memset(&buffer, 0, sizeof(buffer));
    int selectedDescNum = 0;

    while(true){
        if ((selectedDescNum = poll(connectionDescriptors, CLIENT_NUMBER, 10000)) == -1){
            perror("poll(2) error");
            close(sock);
            return -1;
        }

        if (selectedDescNum > 0){

            if (connectionDescriptors[0].revents & POLLIN){
                int newClientFD;
                int id = 0;

                if ((newClientFD = accept(sock, NULL, NULL)) == -1){
                    perror("accept");
                    close(sock);
                    return -1;
                }

                if ((id = getNewID(connectionDescriptors)) == -1){
                    close(newClientFD);
                } else {
                    connectionDescriptors[id].fd = newClientFD;
                    printf("connection with client %d established\n", id);
                }

            }

            for (int i = 1; i < CLIENT_NUMBER; i++){
                if (connectionDescriptors[i].revents & POLLIN){

                    readCount = read(connectionDescriptors[i].fd, buffer, BUFSIZ);
                    if (strlen(buffer) > 0) {
                        printf("client %d: %s\n", i, buffer);
                    }

                    if (readCount == 0){
                        printf("client %d closed connection\n", i);
                        close(connectionDescriptors[i].fd);
                        connectionDescriptors[i].fd = -1;
                    }

                    write(connectionDescriptors[i].fd, buffer, readCount);
                    memset(&buffer, 0, sizeof(buffer));
                }
            }
        }
    }
}
