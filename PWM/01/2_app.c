#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define PWM 'l'
#define PWM_1_ON _IOR(PWM, 11, int)
#define PWM_1_OFF _IOR(PWM, 10, int)
#define PWM_1 _IOR(PWM, 12, int)

int main(int argc, const char *argv[])
{
	if (2 != argc)
	{
		printf("Usage: %s <leddev>\n", argv[0]);
		return -1;
	}
	
	int fd = open(argv[1], O_RDWR);
	
	int cmd;

	while(1)
	{
#if 1
		printf("CMD>>>");
		scanf("%d",&cmd);
		getchar();
		switch (cmd)
		{
		case 10:
			ioctl(fd, PWM_1_OFF);	
			printf("<%d>\n",cmd);
			break;
		case 11:	
			ioctl(fd, PWM_1_ON);
			break;
		case 12:	
			ioctl(fd, PWM_1);
			break;
		}	
#else
		printf("CMD>>>\n");
		scanf("%c",&cmd);
		getchar();
		switch (cmd)
		{
		case '0':
			printf("Enter num >>> ");
			scanf("%d", &num);
			getchar();
			write(fd, &num, sizeof num);
			break;
		case '1':	
			si = read(fd, &num1, sizeof num1);
			printf("%d\n", si);
			if (si == 0)
			{
				printf("Empty ....\n");
				break;
			}
			printf("pop_num = %d\n", num1);
			break;
		case '2':
			return 0;
		}	
#endif
	}
	return 0;
}
