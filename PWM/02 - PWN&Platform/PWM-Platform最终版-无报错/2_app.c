#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define PWM 'l'
#define PWM_1_ON _IOR(PWM, 1, int)
#define PWM_1_OFF _IOR(PWM, 2, int)
#define PWM_1_MUSIC _IOR(PWM, 3, int)

typedef unsigned int u32;

u32 volume[15] = {0, 1912, 1730, 1517, 1432, 1275, 1136, 1012, 965, 851, 758, 715, 637, 568, 506};
int t[27] = {6,6,8,7,6,5,6,3,0,6,5,6,6,8,7,8,6,7,8,0,8,9,10,10,10,10,9};

void delay(u32 Time)
{
	u32 i,j;
	for (i = 0; i < Time; i++)
		for (j = 0; j < 4900; j++)
			;
}

int main(int argc, const char *argv[])
{
	if (2 != argc)
	{
		printf("Usage: %s <leddev>\n", argv[0]);
		return -1;
	}
	
	int fd = open(argv[1], O_RDWR);
	int cmd;
	int tune = 0;
	int loop = 0;

	while(1)
	{
		printf("CMD>>>");
		scanf("%d",&cmd);
		getchar();
		switch (cmd)
		{
		case 1:	
			ioctl(fd, PWM_1_ON);
			break;
		case 2:
			ioctl(fd, PWM_1_OFF);	
			break;
		case 3:	
			for (loop = 0; loop < 27; loop++)
			{
				tune = volume[t[loop]];
				ioctl(fd, PWM_1_MUSIC, &tune);
				delay(9000);
			}
			break;
		}	
	}
	return 0;
}
