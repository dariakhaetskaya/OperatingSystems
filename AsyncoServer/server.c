#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <aio.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>

#define CLIENT_NUMBER 20

int activeClientID = -1;
sigjmp_buf toread;

struct AIO_Block{
    int ClientID;
    struct aiocb block;
    char buffer[BUFSIZ];
};

struct AIO_Block clients[CLIENT_NUMBER];

void asyncRead(int i, int fileDesc){
    clients[i].ClientID = i;
    clients[i].block.aio_fildes = fileDesc;
    clients[i].block.aio_offset = 0;
    clients[i].block.aio_buf = clients[i].buffer;
    clients[i].block.aio_nbytes = BUFSIZ;
    clients[i].block.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    clients[i].block.aio_sigevent.sigev_signo = SIGIO;
    clients[i].block.aio_sigevent.sigev_value.sival_int = i;
    aio_read(&clients[i].block);
}

static void SIGIO_handler(int signo, siginfo_t *siginfo, void *context){
    if (signo != SIGIO || siginfo->si_signo != SIGIO){
        return;
    }
    int index = siginfo->si_value.sival_int;
    if (aio_error(&clients[index].block) == 0){
        activeClientID = index;
        siglongjmp(toread, 1);
    }
}

int getNewID(){
    for (int i = 1; i < CLIENT_NUMBER; i++){
        if (clients[i].ClientID == 0){
            return i;
        }
    }
    return -1;
}

void process(int index){
    ssize_t size = aio_return(&clients[index].block);
    if (size == 0){
        char notification[27];
        sprintf(notification, "client %d closed connection\n", index);
        write(STDOUT_FILENO, notification, sizeof(notification));
        close(clients[index].block.aio_fildes);
        clients[index].ClientID = 0;
    }
    if (size > 0){
        char msg[11];
        sprintf(msg,"client %d: ", index);
        write(STDOUT_FILENO, msg, sizeof(msg));
        for (ssize_t i = 0; i < size; i++){
            clients[index].buffer[i] = toupper(clients[index].buffer[i]);
        }
        write(STDOUT_FILENO, clients[index].buffer, size);
        aio_read(&clients[index].block);
    }
}

int main(){
    memset(clients, 0, CLIENT_NUMBER * sizeof(struct AIO_Block));

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

    unlink(socketPath);

    if (bind(sock, (struct sockaddr *)&socketAddr, sizeof(socketAddr)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }


    if (listen(sock, CLIENT_NUMBER) != 0){
        perror("listen");
        unlink(socketAddr.sun_path);
        close(sock);
        return -1;
    }

    sigset_t sigIO;
    sigaddset(&sigIO, SIGIO);

    struct sigaction sigiohandleraction;
    sigemptyset(&sigiohandleraction.sa_mask);
    sigiohandleraction.sa_sigaction = SIGIO_handler;
    sigiohandleraction.sa_mask = sigIO;
    sigiohandleraction.sa_flags = SA_SIGINFO;
    sigaction(SIGIO, &sigiohandleraction, NULL);

    if(sigsetjmp(toread, 1) == 1 && activeClientID > 0){
        process(activeClientID);
    }

    while (true){
	int newClientFD;
	if ((newClientFD = accept(sock, NULL, NULL)) == -1){
    		if (errno == EINTR){
			printf("interrupted\n");
        		continue;
    		} else {
        		perror("accept");
       	 		continue;
    		}
	}
	int newClientID;
	if ((newClientID = getNewID()) != -1){
    		asyncRead(newClientID, newClientFD);
	} else {
    		printf("server is busy atm, try again later\n");
	}
    }
}
