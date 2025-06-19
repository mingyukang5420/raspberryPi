// daemon.c

#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>

int main (int argc, char *argv[]){
    struct sigaction sa;
    struct rlimit rl;
    int fd0, fd1, fd2, i;
    pid_t pid;

    if(argc < 2){
        printf("Usage: %s command\n", argv[0]);
        return -1;
    }

    /* set the umask to 0 for creating files with the mode 0666 */
    umask(0);

    /* get the maximum number of file descriptors */
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0){
        perror("getrlimit()");
    }

    if((pid = fork()) < 0){
        perror("fork()");
    } else if (pid != 0){   /* parent process would be terminated */
        return 0;
    }

    /* become session leader for detaching from the controlling terminal */
    setsid();

    /* ignore the signals which control the terminal */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGCHLD, &sa, NULL) < 0){
        perror("sigaction() : Can't ignore SIGHUP");
    }

    /* set the working directory to the root directory */
    if(chdir("/") < 0){
        perror("chdir()");
    }

    /* close all open file descriptors */
    if(rl.rlim_max == RLIM_INFINITY){
        rl.rlim_max = 1024;
    }

    for(i = 0; i < rl.rlim_max; i++){
        close(i);
    }

    /* connect stdin, stdout, stderr to /dev/null */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    /* open the log file to print the logs*/
    openlog(argv[1], LOG_CONS, LOG_DAEMON);
    if(fd0 != 0 || fd1 != 1 || fd2 != 2){
        syslog(LOG_ERR, "unexpected file descriptors %d, %d, %d", fd0, fd1, fd2);
        return -1;
    }

    /* print the info level logs to log file*/
    syslog(LOG_INFO, "Daemon Process");

    while(1){
        /* repeat the process */
    }

    /* close the system log */
    closelog();
    return 0;
}
