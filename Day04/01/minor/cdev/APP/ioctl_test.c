#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CLEARQ    _IO('Q', 0)
#define GETNUMQ  _IOR('Q', 1, int)

int main(int argc, char **argv)
{
	if(3 != argc)
	{
		printf("Usage: %s <dev> <C/G>\n", argv[0]);
		return -1;
	}

	int fd = open(argv[1], O_RDWR);
	if(-1 == fd)
	{
		perror("open");
		return -1;
	}

	int num;
	switch(argv[2][0])
	{
	case 'C':
		ioctl(fd, CLEARQ);
		break;
	case 'G':
		ioctl(fd, GETNUMQ, &num);
		printf("num: %d\n", num);
		break;
	default:
		printf("unkown !\n");
	}
}
