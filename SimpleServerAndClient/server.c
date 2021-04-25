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

    /*На всякий случай вызываем функцию unlink,
     * чтобы гарантировать отсутствие сгенерированного
     * имени в файловой системе. */

    unlink(socketPath);

    /*присвоить имя дескриптору */
    if (bind(sock, (struct sockaddr *)&socketAddr, sizeof(socketAddr)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    /* сообщить ядру, что процесс является сервером */
    if (listen(sock, 1) != 0){
        perror("listen");
        unlink(socketAddr.sun_path);
        close(sock);
        return -1;
    }

    int connectionDesc;

    if ((connectionDesc = accept(sock, NULL, NULL)) == -1){
        perror("listen");
        unlink(socketAddr.sun_path);
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
        unlink(socketAddr.sun_path);
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
    unlink(socketAddr.sun_path);
    return 0;
}
