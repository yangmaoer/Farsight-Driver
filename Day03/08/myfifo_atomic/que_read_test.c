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

	//void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
	// 返回 - 映射到用户空间的地址
		//addr - 指定文件应被映射到用户空间的起始地址	-	NULL
		//len - 映射到调用用户空间的字节数
				//从被映射文件开头offset个字节开始算起	-	0
		//prot - 指定访问权限
		//flags - 进程间共享区域
	char *p = mmap(NULL, 1 << 12, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(MAP_FAILED == p)
	{	
		perror("mmap");
		return -1;
	}	
	
	printf("%s\n", p);
}
