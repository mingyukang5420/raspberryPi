//tcp_server.c

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// # define TCP_PORT 5100           // define port number 

int main(int argc, char *argv[]){
    int ssock; // server socket descriptor
    u_int16_t TCP_PORT;
    
    socklen_t clen; // length of client address
    struct sockaddr_in serv_addr, clnt_addr; // structure of server and client
    char mesg[BUFSIZ]; // message buffer

    if (argc<2){ // print error for not enough arguments
        printf("Usage : %s PORT_NUMBER\n", argv[0]);
        return -1;
    }

    if (sscanf(argv[1], "%hu", &TCP_PORT) != 1){
        fprintf(stderr, "Error : Invalid port number '%s'.\n", argv[1]);
        return -1;
    }
    // port number validation
    if(TCP_PORT == 0){
        fprintf(stderr, "Error : Port number cannot be 0.\n");
        return -1;
    }

    if(TCP_PORT < 5000){
        printf("Consider using a port number greater than or equal to 5000.\n \
            example : 5100, 5101, 5102, 8080, 8888");
    }

    if(TCP_PORT < 1024){
        printf("Warning : Using a port number less than 1024 may require root privileges.\n");
        exit(EXIT_FAILURE);
    }

    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ // create socket
        perror("socket()"); // error message for failure of socket creation
        return -1;
}

memset(&serv_addr, 0, sizeof(serv_addr)); // initializing structure of server
serv_addr.sin_family = AF_INET; // set server address family as IPv4
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // receive any network interface
serv_addr.sin_port = htons(TCP_PORT); // set the port number

if(bind(ssock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ // bind the address to socket
    perror("bind()"); // error message for failure of bind func
    return -1;
}

if(listen(ssock, 8) < 0){ // transition the sockets as waiting // waiting queue for connect at most 8
    perror("listen()"); // error message for failure waiting receiving
    close(ssock);
    return -1;
}

printf("Server listening on port %hu\n",TCP_PORT);

clen = sizeof(clnt_addr); // initializing length of client address
while(1){
    int bytes_received;
    char client_ip_str[INET_ADDRSTRLEN]; // buffer for saving the client ip address
    
    printf("\nWaiting for client connection...\n");     // waiting message
    
    int csock = accept(ssock, (struct sockaddr *)&clnt_addr, &clen); // accept the connection of client
    if (csock < 0){
        perror("accept()");
        continue;   // if accept error happened it would be waiting for next connection
    }

    inet_ntop(AF_INET, &clnt_addr.sin_addr, client_ip_str, sizeof(client_ip_str)); // transfer the client address to string
    printf("Client is connected from %s:%hu\n", client_ip_str, ntohs(clnt_addr.sin_port)); // print the info of connection with client

    while((bytes_received = read(csock, mesg, BUFSIZ)) > 0){ // if there exists message received
        printf("Received data from %s: %s\n", client_ip_str, mesg); // print what server received and where the message came from

        // server shutdown command
        if (strncmp(mesg, "/server_quit", 12) == 0){
            printf("Server received '/server_quit'. Shutting down...\n");
            close(csock); // close the current client socket
            close(ssock); // close the socket listening
            return 0;   // finish the server process
        }


        if(write(csock, mesg, bytes_received) <= 0){ // send the message to client (echo)
        perror("write()"); // error message for failure of write func
        break;
        }

    }
    
    if(bytes_received == 0){ // it means that client closed normally
        printf("Client %s disconnected normally.\n", client_ip_str);
    } else if( bytes_received < 0){
        perror("read()"); // error message for failure of read func
    }
    close(csock); // close the socket of client
}
return 0; // end of the program
}