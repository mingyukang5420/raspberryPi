// client.c

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>         // to handle signal
#include <sys/wait.h>       // to use waitpid()

// #define TCP_PORT 5100

// declaration for global variables
// the g_ in front of the variables means that it is global variable
static int g_pfd[2];        // g_pfd : array of the pipe file descriptor (g_pfd[0] to read, g_pfd[1] to write)
static int g_sockfd;        // g_sockfd : server socket file descriptor (would be accessable both parent and child client)
static volatile int g_control = 1;       // g_control : control flag to shutdown (volatile is more safe when signalHandler uses)

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define CLEAR_LINE "\r\033[K"

void sigHandler(int signo){
    if (signo == SIGUSR1){      // signal that child process sends message
        char buf[BUFSIZ];
        int n;

        n = read(g_pfd[0], buf, BUFSIZ);    // g_pfd[0]is the end of reading pipe
        if (n > 0){
            // send read message to server socket
            if (write(g_sockfd, buf, n) < 0) {   // g_sockfd is server socket file descriptor
                perror("send() in sigHandler");
                g_control = 0;      // finish client if error outbreaks
            }
         }  else if (n == 0){     // case of being close pipe to write (it means that child process is ended)
                fprintf(stderr, "\nChild process pipe closed. Client exiting.\n");
                g_control = 0;      // set the flag for exit flag
        } else {
                perror("read() in sigHandler");
                g_control = 0;      // finish client when error outbreaks
        }
    } else if (signo == SIGCHLD){   //  signal when child process terminated
        // it provides being zombie process to wait the termination status of child process
        wait(NULL);     // take the all of killed child process (if there exist more, use while loop)
        fprintf(stderr, "\nChilde process terminated. Client exiting.\n");
        g_control = 0;  // termination of the main loop from parent process
    } else if (signo == SIGINT){   // to handle Ctrl + C (SIGINT) signal 
        // set the flag to terminate both parent and child client process
        fprintf(stderr, "\nSIGINT (Ctrl+C) received. Client exiting gracefully.\n");
        g_control = 0;
    }
}

int main (int argc, char *argv[]){
    u_int16_t TCP_PORT;
    //int g_sockfd;  // server socket
    struct sockaddr_in serv_addr;
    char mesg_received[BUFSIZ];
    char mesg_sending[BUFSIZ];
    int pid;  // to save the return value of the function fork()

    // main_part1 : validation for argument
    if (argc<3){
        printf("Usage : %s IP_ADDRESS PORT_NUMBER\n", argv[0]);
        return -1;
    }

    if (sscanf(argv[2], "%hu", &TCP_PORT) != 1){
        fprintf(stderr, "Error : Invalid port number '%s'.\n", argv[2]);
        return -1;
    }
    // port number validation
    if(TCP_PORT == 0){
        fprintf(stderr, "Error : Port number cannot be 0.\n");
        return -1;
    }

    if(TCP_PORT < 5000){
        printf("Consider using a port number greater than or equal to 5000.\n \
            example : 5100, 5101, 5102, 8080, 8888\n");
    }

    if(TCP_PORT < 1024){
        printf("Warning : Using a port number less than 1024 may require root privileges.\n");
        exit(EXIT_FAILURE);
    }

    // main_part2 : create server socket
    if((g_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket()");
        return -1;
    }
    // initialize server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    // convert IP address string to binary form
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr.s_addr) <= 0){
        perror("inet_pton()");
        close(g_sockfd);
        return -1;
    }
    serv_addr.sin_port = htons(TCP_PORT);  // set port number
    
    // main_part3 : connect server
    printf("Connecting to server at %s:%hu...\n",argv[1], TCP_PORT);
    if(connect(g_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("connect()");
        close(g_sockfd);
        return -1;
    }

    printf("Connected succesfully. Type your message('/quit' to exit).\n");

    // the start point of splitting input and output using fork()

    // fork_part1 : make pipe : to send the message from child to parent
    if (pipe(g_pfd) == -1){
        perror("pipe()");
        close(g_sockfd);    // close the server socket when pipe couldn't be made
        return -1;
    }

    // fork_part2 : separate the parent and child to use fork() function
    if((pid = fork())<0){  // error case of fork() function
        perror("fork()");
        close(g_sockfd);
        return -1;
    } 
    // fork_part3 : child process
    else if (pid == 0){  // child process case
        close(g_pfd[0]); // child process doesn't need to use reading pipe
        close(g_sockfd); // child process doesn't need to contact with server
        
        //signal(SIGCHLD, sigHandler); // if it has more child it could be on
        signal(SIGINT, SIG_DFL); // since parent process have sigHandler for SIGINT,
        //child process have to use this signal function

        while(g_control){
            printf(COLOR_BLUE "You: " COLOR_RESET);     // print the text to lead user input
            fflush(stdout);

            // read what user typed
            if (fgets(mesg_sending, sizeof(mesg_sending), stdin) == NULL){
                printf(CLEAR_LINE "\nEOF (Ctrl+D) received. Disconnecting...\n");
                g_control = 0;      // set termination flag when receiving EOF
            }

            // check whether message is '/quit' or not 
            if (strcmp(mesg_sending, "/quit") == 0){
                printf(CLEAR_LINE "Exiting chat as requested by '/quit'.\n");
                g_control = 0;      // set termination flag
            }
            // send the message input to g_pfd[1]
            // mesg_sending including '\0'
            if(write (g_pfd[1], mesg_sending, strlen(mesg_sending) + 1) == -1 ){ 
                perror("write to pipe");
                g_control = 0;
            }

            kill(getppid(), SIGUSR1);   // send SIGUSR1 signal to parent process that there exists message to send

            if (!g_control) break;
        }
        close(g_pfd[1]);    // close the pipe to write
        exit(0);            // terminate child process
    }
    // fork_part4 : parent process
    else {  // parent process to contact with server and print message received
    close(g_pfd[1]);    

    signal(SIGUSR1, sigHandler);  // signal that child sending message
    signal(SIGCHLD, sigHandler);  // signal that child terminated
    signal(SIGINT, sigHandler);   // signal that Ctrl + C interrupted

    while (g_control){
        // receive the message from the server (blocking mode)
        memset(mesg_received, 0, BUFSIZ);  // initializing

        // to preprocess NULL
        int bytes_received = recv(g_sockfd, mesg_received, BUFSIZ - 1, 0);

        // to clear the current console screen
        write(1, CLEAR_LINE, strlen(CLEAR_LINE));

        if (bytes_received < 0){
            perror("recv() from server in Parent");
            g_control = 0;
        } else if (bytes_received == 0) { // when server is terminated normally
            printf("Server disconnected.\n");
            g_control = 0;
        } else {
            mesg_received[bytes_received] = '\0'; // add NULL at the end of received message
            printf(COLOR_GREEN "Server: %s\n" COLOR_RESET, mesg_received);  // print the server message

            // to print user input prompt after sending server message
            printf(COLOR_BLUE "You: " COLOR_RESET);
            fflush(stdout); // clear the buffer to show prompt immediately
        }
    }

    // fork_part5 : termination of the program and free the resources
    close(g_pfd[0]);

    if (pid > 0 && waitpid(pid, NULL, WNOHANG) == 0){
        kill(pid, SIGCHLD);     // lead the child process to be terminated sending SIGCHLD signal
        waitpid(pid, NULL, 0);  // prevent child process being zombie process
    }
    }
    close(g_sockfd);
    printf("Client program terminated.\n");
    return 0;
}