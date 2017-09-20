#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

char buf[1024] = {0};

int main(int argc, char **argv)
{
	if(2 != argc)
	{
		printf("Usage: %s <file>\n", argv[0]);
		return -1;
	}

	int fd = open(argv[1], O_RDWR);
	if(-1 == fd)
	{
		perror("open");
		return -1;
	}

	char *p = mmap(NULL, 1<<12, PROT_WRITE, MAP_SHARED, fd, 0);
	if(MAP_FAILED == p)
	{	
		perror("mmap");
		return -1;
	}	
	
	while(1)
	{

		*( (int*)(p+0x0c44) ) = 0xff; //GPX2DAT
		sync();
		sleep(1);

		*( (int*)(p+0x0c44) ) = 0;
		sync();
		sleep(1);
		
	}
}
