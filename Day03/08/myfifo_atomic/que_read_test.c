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
	// ���� - ӳ�䵽�û��ռ�ĵ�ַ
		//addr - ָ���ļ�Ӧ��ӳ�䵽�û��ռ����ʼ��ַ	-	NULL
		//len - ӳ�䵽�����û��ռ���ֽ���
				//�ӱ�ӳ���ļ���ͷoffset���ֽڿ�ʼ����	-	0
		//prot - ָ������Ȩ��
		//flags - ���̼乲������
	char *p = mmap(NULL, 1 << 12, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(MAP_FAILED == p)
	{	
		perror("mmap");
		return -1;
	}	
	
	printf("%s\n", p);
}
