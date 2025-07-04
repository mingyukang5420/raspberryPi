// epoll_server.c

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define SERVER_PORT 5100
#define MAX_EVENT   32

void setnonblock(int fd){
    int opts = fcntl(fd, F_GETFL);
    opts |= O_NONBLOCK;
    fcntl(fd, F_SETFL, opts);
}

int main(int argc, char *argv[]){
    int ssock, csock;
    socklen_t clen;
    int n, epfd, nfds = 1;
    struct sockaddr_in servaddr, cliaddr;
    struct epoll_event ev, events[MAX_EVENT];
    char mesg[BUFSIZ];

    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket()");
        return -1;
    }

    setnonblock(ssock);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("bind()");
        return -1;
    }

    if(listen(ssock, 8) < 0){
        perror("listen()");
        return -1;
    }

    epfd = epoll_create(MAX_EVENT);
    if(epfd == -1){
        perror("epoll_create()");
        return -1;
    }

    ev.events = EPOLLIN;
    ev.data.fd = ssock;

    if(epoll_ctl(epfd, EPOLL_CTL_ADD, ssock, &ev) == -1){
        perror("epoll_ctl()");
        return -1;
    }

    do{
        epoll_wait(epfd, events, MAX_EVENT, 500);
        for(int i = 0; i < nfds; i++){
            if(events[i].data.fd == ssock){
                clen = sizeof(struct sockaddr_in);
                
                if((csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen)) <= 0){
                    perror("accept()");
                    continue;
                }
                else{
                    inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
                    printf("New client connected: %s\n", mesg);

                    setnonblock(csock);

                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = csock;

                    epoll_ctl(epfd, EPOLL_CTL_ADD, csock, &ev);
                    nfds++;
                    continue;
                }
                }else if(events[i].events & EPOLLIN){
                    if(events[i].data.fd <0) continue;

                    memset(mesg, 0, sizeof(mesg));
                    if((n = read(events[i].data.fd, mesg, sizeof(mesg))) > 0){
                        printf("Received message: %s", mesg);
                        write(events[i].data.fd, mesg, n);
                        close(events[i].data.fd);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                        nfds--;
                    }
                }
            }
        }
        while(strncmp(mesg, "q", 1));
    close(ssock);
    close(epfd);
    return 0;
}

