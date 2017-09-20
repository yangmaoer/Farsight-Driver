#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LED_ON   _IOW('L', 0, int)
#define LED_OFF  _IOW('L', 1, int)

int main(int argc, char **argv)
{
	if(3 != argc)
	{
		printf("Usage: %s <dev> <k/g>\n", argv[0]);
		return -1;
	}

	int fd = open(argv[1], O_RDWR);
	if(-1 == fd)
	{
		perror("open");
		return -1;
	}

	ioctl(fd, argv[2][0]=='k'?LED_ON:LED_OFF);
}
