#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void){
//	close(0);
//	int n;
	int fd1, fd2;
	char str[BUFSIZ];
//	scanf("%d", &n);
	
//	close(1);
	read(0, str, BUFSIZ);
	printf("Hello, world(%s)\n", str);

//	printf("Hello, world(%d)\n",n);

	fd1 = open("sample1.txt", O_WRONLY | O_CREAT);
	fd2 = open("sample2.txt", O_WRONLY | O_CREAT);
	printf("%d %d\n", fd1, fd2);
	close(fd1);
	close(fd2);

	return 0;
}

