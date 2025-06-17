#include <stdio.h>
#include <unistd.h>

static int g_var = 1;
char str[] = "PID";

int main (int argc, char *argv[]){
    int var;
    pid_t pid;
    var = 92;

    if((pid = vfork()) < 0){
        perror("[Error] : vfork()");
        // exit(1);
    }else if(pid == 0){
        g_var++;
        var++;
        printf("Parent %s from Child Process(%d) : %d \n", str, getpid(), getppid());
		printf("pid = %d, Global var = %d, var = %d\n", getpid(), g_var, var);
		_exit(0);
    }else{
        printf("Child %s from Parent Process(%d) : %d \n", str, getpid(), pid);
	// sleep(1);
    }

    printf("pid = %d, Global var = %d, var = %d\n", getpid(), g_var, var);
    // exit(0);
    return 0;
}
