#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <netdb.h>
#include <sys/signal.h>

bool connectionAlive = false;

static void sig_usr(int signo){
    if (signo == SIGPIPE){
        connectionAlive = false;
    }
}


int main(int argc, char* argv[]){

    if (sigset(SIGPIPE, sig_usr) == SIG_ERR){
        perror("failed to handle SIGPIPE");
        exit(1);
    }

    in_port_t destinationPort;

    if (argc == 2){
        destinationPort = (in_port_t)atoi(argv[1]);
    } else {
        printf("Incorrect arguments, client was initialized on default port 1888\n");
        destinationPort = 1888;
    }

    struct sockaddr_in DestinationAddress;
    memset(&DestinationAddress, 0 , sizeof(struct sockaddr_in));
    DestinationAddress.sin_family = AF_INET;
    DestinationAddress.sin_port = htons(destinationPort);
    struct in_addr HostAddress = *(struct in_addr*)gethostbyname("localhost")->h_addr_list[0];
    memcpy(&DestinationAddress.sin_addr, &HostAddress, sizeof(struct in_addr));

    int sock;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return -1;
    }

    if (connect(sock, (struct sockaddr* )&DestinationAddress, sizeof(struct sockaddr_in)) == -1){
        perror("connection error");
        close(sock);
        return -1;
    } else {
        printf("connected\n");
        connectionAlive = true;
    }

    char readWriteBuf[BUFSIZ];
    char tempBuf[BUFSIZ];
    int readCount;
    int writeCount;

    while ((readCount = read(STDIN_FILENO, readWriteBuf, BUFSIZ)) > 0){
        if (!connectionAlive){
            close(sock);
            printf("connection was closed\n");
            return 0;
        }
        while ((writeCount = write(sock, readWriteBuf, readCount)) < readCount){
            strncpy(tempBuf, readWriteBuf + writeCount, readCount - writeCount);
            memset(&readWriteBuf, 0, sizeof(readWriteBuf));
            strncpy(readWriteBuf, tempBuf, readCount - writeCount);
            memset(&tempBuf, 0, sizeof(tempBuf));
            readCount -= writeCount;
        }

        if (writeCount == 0){
            printf("connection was closed\n");
            close(sock);
            return 0;
        }

        if (writeCount == -1){
            perror("writing error");
            return -1;
        }
    }

    if (readCount == -1){
        perror("reading error");
        return -1;
    }

    close(sock);
    return 0;

}
