#include <stdio.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
int main(int argc, char **argv)
{
	if(2 != argc)
	{
		printf("Usage: %s <randomdev>\n", argv[0]);
		return -1;
	}

	int fd = open(argv[1], O_RDWR);

	int num;
	read(fd, &num, sizeof num);

	printf("%d\n", num);


	ioctl(fd, 0, 1000);

	ioctl(fd, 1, &num);
	printf("num = %d\n", num);
}
