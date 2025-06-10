#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>          // perror() 함수
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    
    int n, in, out;
    char buf[1024];
    /* 명령행 인수로 append할 파일명이 없는 경우에 에러를 출력하고 종료한다. */
    if (argc < 3) {
        write(2, "Usage : append file1 file2\n", 25);
        return -1;
    }

    /* append의 원본이 되는 파일을 읽기 모드로 연다. */
    if ((in = open(argv[1], O_RDONLY)) < 0){
        perror(argv[1]);
        return -1;
    }
	/* 병합할 파일은 읽기, 쓰기 모드로 연다. */
    if((out = open(argv[2], O_RDWR, S_IRUSR | S_IWUSR)) < 0){
        perror(argv[2]);
        return -1;
    }

    while((n = read(in, buf, sizeof(buf))) > 0){
        write(out, buf, n);
    }

    close(out);
    close(in);

    return 0;
}
