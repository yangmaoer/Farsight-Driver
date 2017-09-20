#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

/*
       void *mmap(void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset);
       int munmap(void *addr, size_t length);
*/
int main(int argc, char **argv)
{
	if(2 != argc)
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}

	int fd = open(argv[1], O_RDWR);
	if(-1 == fd)
	{
		perror("open");
		return -1;
	}

	char *p = mmap(NULL, 100, PROT_WRITE, MAP_SHARED, fd, 0);
	if(MAP_FAILED == p)
	{
		perror("mmap");
		return -1;
	}
	
	strcpy(p, "hello");
	
	munmap(p, 100);
}
