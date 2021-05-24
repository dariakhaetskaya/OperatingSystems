#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <math.h>
#include <netdb.h>
#include <arpa/inet.h>

#define CLIENT_NUMBER 512

int getNewID(struct pollfd *connectionDescriptors){
    for (int i = 1; i < 2 * CLIENT_NUMBER; i++){
        if (connectionDescriptors[i].fd == -1){
            return ceil(i/2.0);
        }
    }
    return -1;
}

int main(int argc, char* argv[]){

    if (argc < 4) {
        printf("arguments: [1: source port, 2: destinationPort, 3: host]\n");
        return -1;
    }
    printf("%d\n", getpid());
    in_port_t proxyPort = (in_port_t)atoi(argv[1]);
    in_port_t destinationPort = (in_port_t)atoi(argv[2]);

    struct hostent* HostInfo = gethostbyname(argv[3]);
    struct in_addr HostAddr = *(struct in_addr*) HostInfo->h_addr_list[0];

    printf("proxyPort: %d\n", proxyPort);
    printf("destinationPort: %d\n", destinationPort);
    printf("HostAddr: %d\n", HostAddr.s_addr);

    int sock;
    struct sockaddr_in socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(proxyPort);
    socketAddr.sin_addr.s_addr = INADDR_ANY;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return -1;
    }

    int FlagVal = 1;

    if(setsockopt(sock, SOL_SOCKET,SO_REUSEADDR, &FlagVal, sizeof(int)) == -1)
    {
        perror("setsockopt");
        return -1;
    }

    /*присвоить имя дескриптору */
    if (bind(sock, (struct sockaddr *)&socketAddr, sizeof(socketAddr)) < 0) {
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

    struct pollfd connectionDescriptors[2 * CLIENT_NUMBER];

    // initialization
    for (int i = 0; i < 2 * CLIENT_NUMBER; i++){
        connectionDescriptors[i].fd = -1;
        connectionDescriptors[i].events = POLLIN;
        connectionDescriptors[i].revents = 0;
    }

    connectionDescriptors[1].fd = sock;
    int takenDescriptorsCount = 1; // one desc in taken for socket

    ssize_t readCount = 0;
    char buffer[BUFSIZ];

    int selectedDescNum = 0;

    while(true){
        if ((selectedDescNum = poll(connectionDescriptors, 2 * CLIENT_NUMBER, 10000)) == -1){
            perror("poll(2) error");
            close(sock);
            return -1;
        }

        if (selectedDescNum > 0){

            if (connectionDescriptors[1].revents & POLLIN){
                int newClientFD;
                int id = 0;

                if ((newClientFD = accept(sock, NULL, NULL)) == -1){
                    perror("accept");
                    continue;
                }

                printf("newClientFD: %d\n", newClientFD);

                if ((id = getNewID(connectionDescriptors)) == -1){
                    close(newClientFD);
                } else {
                    printf("newClientID: %d\n", id);
                    struct sockaddr_in destinationAddr;
                    memset(&destinationAddr, 0, sizeof(struct sockaddr_in));
                    destinationAddr.sin_family = AF_INET;
                    destinationAddr.sin_port = htons(destinationPort);
                    memcpy(&destinationAddr.sin_addr, &HostAddr, sizeof(struct in_addr));

                    int destinationSocket;

                    if ((destinationSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
                        perror("socket");
                        close(sock);
                        return -1;
                    }

                    if (connect(destinationSocket, (struct sockaddr*)&destinationAddr, sizeof(struct sockaddr_in)) == -1){
                        perror("connect");
                        close(sock);
                        return -1;
                    }

                    connectionDescriptors[2*id].fd = newClientFD;
                    connectionDescriptors[2*id + 1].fd = destinationSocket;
                    takenDescriptorsCount += 2;
                    printf("connection established\n");
                }

            }

            for (int i = 2; i < 2 * CLIENT_NUMBER; i+= 2){
                if (connectionDescriptors[i].revents & POLLIN){
                    readCount = read(connectionDescriptors[i].fd, buffer, BUFSIZ);
                    if (strlen(buffer) > 0){
                        printf("client %d: %s\n", i/2, buffer);
                    }

                    if (readCount == 0){
                        printf("client %d closed connection\n", i/2);
                        close(connectionDescriptors[i].fd);
                        close(connectionDescriptors[i+1].fd);
                        connectionDescriptors[i].fd = -1;
                        connectionDescriptors[i+1].fd = -1;
                    }

                    write(connectionDescriptors[i+1].fd, buffer, readCount);
                    memset(&buffer, 0, sizeof(buffer));
                }
            }

            for (int i = 3; i < 2 * CLIENT_NUMBER; i+= 2){
                if (connectionDescriptors[i].revents & POLLIN){
                    readCount = read(connectionDescriptors[i].fd, buffer, BUFSIZ);
                    if (strlen(buffer) > 0) {
                        printf("server %d: %s\n", (i - 1) / 2, buffer);
                    }

                    if (readCount == 0){
                        printf("server %d closed connection\n", (i - 1) / 2);
                        close(connectionDescriptors[i].fd);
                        close(connectionDescriptors[i-1].fd);
                        connectionDescriptors[i].fd = -1;
                        connectionDescriptors[i-1].fd = -1;
                    }

                    write(connectionDescriptors[i-1].fd, buffer, readCount);
                    memset(&buffer, 0, sizeof(buffer));
                }
            }
        }
    }
}
