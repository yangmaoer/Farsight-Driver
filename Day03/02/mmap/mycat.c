#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char buf[1024] = {0};

int main(int argc, char **argv)
{
	if(2 != argc)
	{
		printf("Usage: %s <file>\n", argv[0]);
		return -1;
	}

	int fd = open(argv[1], O_RDONLY);
	if(-1 == fd)
	{
		perror("open");
		return -1;
	}

	read(fd, buf, sizeof buf);

	printf("%s\n", buf);
}
