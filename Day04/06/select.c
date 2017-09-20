#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	if(4 != argc)
	{
		printf("Usage: %s <file1> <file2> <file3\n", argv[0]);
		return -1;
	}
	
	int fd1 = open(argv[1], O_RDWR);
	int fd2 = open(argv[2], O_RDWR);
	int fd3 = open(argv[3], O_RDWR);

	char buf[100];

	while(1)
	{
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(fd1, &readfds);
		FD_SET(fd2, &readfds);
		FD_SET(fd3, &readfds);

		select(fd3+1, &readfds, NULL, NULL, NULL);
	
		if(FD_ISSET(fd1, &readfds))
		{
			read(fd1, buf, sizeof buf);
			printf("read1: %s\n", buf);
		}

		if(FD_ISSET(fd2, &readfds))
		{
			read(fd2, buf, sizeof buf);
			printf("read2: %s\n", buf);
		}

		if(FD_ISSET(fd3, &readfds))
		{
			read(fd3, buf, sizeof buf);
			printf("read3: %s\n", buf);
		}
	}

}
