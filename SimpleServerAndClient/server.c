#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

void printInUpper(const char* string, int readCount){
    char Char;
    for(int i = 0; i < readCount; i++){
        Char = string[i];
        putchar(toupper(Char));
    }
}

int main(){

    int sock;
    struct sockaddr_un socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sun_family = AF_UNIX;
    char* socketPath = "./socket";
    strcpy(socketAddr.sun_path, socketPath);

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return -1;
    }

    /*присвоить имя дескриптору */
    if (bind(sock, (struct sockaddr *)&socketAddr, sizeof(socketAddr)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    /* сообщить ядру, что процесс является сервером */
    if (listen(sock, 1) != 0){
        perror("listen");
        close(sock);
        return -1;
    }
    
    int connectionDesc = accept(sock, NULL, NULL);
    unlink(socketAddr.sun_path);
    if (connectionDesc == -1){
        perror("listen");
        close(connectionDesc);
        close(sock);
        return -1;
    }

    printf("connection established\n");
    connectionAlive = true;
    int readCount = 0;
    char buffer[BUFSIZ];

    while ((readCount = read(connectionDesc, buffer, BUFSIZ)) > 0){
	printInUpper(buffer, readCount);
	memset(&buffer, 0, sizeof(buffer));
    }

    if (readCount == -1){
        perror("reading error");
        close(connectionDesc);
        close(sock);
        return -1;
    }
    if (readCount == 0){
        printf("connection was closed\n");
    }

    printf("\n");
    close(connectionDesc);
    close(sock);
    return 0;
}
