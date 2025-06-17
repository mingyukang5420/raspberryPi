#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define UDP_PORT 5100

int main(int argc, char **argv) {
    int sockfd, n;
    struct sockaddr_in serv_addr, cliaddr;
    socklen_t len;
    char mesg[1000];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(UDP_PORT);
    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    do{
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, mesg, 1000, 0, (struct sockaddr *)&cliaddr, &len);
        sendto(sockfd, mesg, n, 0, (struct sockaddr *)&cliaddr, len);

        mesg[n] = 0;
        printf("Received message : %s\n", mesg);
    } while(strcmp(mesg, "quit"));

    close(sockfd);
    return 0;
}
