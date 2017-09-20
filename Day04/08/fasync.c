#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

int fd;

void mygets(int num)
{
	printf("sig! \n");
	char buf[100] = {0};
	read(fd, buf, sizeof buf);
	printf("get: %s\n", buf);
	return ;
}

int main(int argc, char **argv)
{
	if(2 != argc)
	{
		printf("Usage: %s <file>\n", argv[1]);
		return -1;
	}

	fd = open(argv[1], O_RDWR);

	
	fcntl(fd, F_SETOWN, getpid());
	int flag = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flag | FASYNC);
	signal(SIGIO, mygets);

	while(1)
	{
//		printf("aaaaaaaaaaaaaaa\n");
	}
}
