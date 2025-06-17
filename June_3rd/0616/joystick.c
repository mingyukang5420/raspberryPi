// joystick.c

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>

#define JOY_DEV "/dev/input/event2"

int main(void){
    int fd;
    struct input_event ie;

    if((fd=open(JOY_DEV, O_RDONLY))==-1){
        perror("opening device");
        exit(EXIT_FAILURE);
}

    while(1){
        read(fd, &ie, sizeof(struct input_event));
        printf("time %ld.%06ld,\ttype %d,\tcode %-3d,\tvalue %d\n",   
        ie.time.tv_sec, ie.time.tv_usec, ie.type, ie.code, ie.value);

        if(ie.type){
            switch(ie.code){
                case KEY_UP:
                    printf("UP\n");
                    break;
                case KEY_DOWN:
                    printf("DOWN\n");
                    break;
                case KEY_LEFT:
                    printf("LEFT\n");
                    break;
                case KEY_RIGHT:
                    printf("RIGHT\n");
                    break;
                case KEY_ENTER:
                    printf("ENTER\n");
                    break;
                default:
                    printf("Default\n");
                    break;
            }
        }
    }

    close(fd);
    return 0;
}
