#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>


int main(int argc, char **argv)
{
	if(2 != argc)
	{
		printf("Usage: %s <file>\n", argv[0]);
		return -1;
	}
	//int fd = open(argv[1], O_RDWR);
	int fd = open(argv[1], O_RDWR | O_NONBLOCK);

	char buf[10];
	read(fd, buf, sizeof buf);

	printf("over!\n");
}
