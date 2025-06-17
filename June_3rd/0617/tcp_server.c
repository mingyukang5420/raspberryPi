//tcp_server.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

# define TCP_PORT 5100

int main(int argc, char *argv[]){
    int ssock;
    socklen_t clen;
    struct sockaddr_in serv_addr, clnt_addr;
    char mesg[BUFSIZ];

    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket()");
        return -1;
}

memset(&serv_addr, 0, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
serv_addr.sin_port = htons(TCP_PORT);

if(bind(ssock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
    perror("bind()");
    return -1;
}

if(listen(ssock, 8) < 0){
    perror("listen()");
    return -1;
}

clen = sizeof(clnt_addr);
do{
    int n, csock = accept(ssock, (struct sockaddr *)&clnt_addr, &clen);

    inet_ntop(AF_INET, &clnt_addr.sin_addr, mesg, BUFSIZ);
    printf("Client is connected from %s\n", mesg);

    if((n = read(csock, mesg, BUFSIZ)) <= 0){
        perror("read()");
}

printf("Received data: %s\n", mesg);

if(write(csock, mesg, n) <= 0){
    perror("write()");}

    close(csock);
} while(strncmp(mesg, "q",1) );

close(ssock);
return 0;
}
