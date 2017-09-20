#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define RTC 'R'
#define RTC_DISPLAY _IOR(RTC, 0, int)
#define RTC_SET     _IOW(RTC, 1, int)
#define RTC_UNDIS   _IOR(RTC, 2, int)

typedef unsigned int u32;

int main(int argc, const char *argv[])
{
	if (2 != argc)
	{
		printf("Usage: %s <RTC-cDev>\n", argv[0]);
		return -1;
	}
	
	int fd = open(argv[1], O_RDWR);
	
	int cmd;
	u32 sec;
	while(1)
	{
		int t = 60;
		int g, s;
		printf("CMD>>>");
		scanf("%d",&cmd);
		getchar();
		switch (cmd)
		{
		case 0:
			while(t--)
			{
				ioctl(fd, RTC_DISPLAY, &sec);	
				g = sec % 16;
				s = (sec - g) / 16;

				printf("%d%d\t", s, g);
				fflush(NULL);
				sleep(1);
			}
			putchar(10);
			break;
		case 1:	
			ioctl(fd, RTC_SET, sec);
			break;
		case 2:
			ioctl(fd, RTC_UNDIS, sec);
			break;
		break;
		}	
	}
	return 0;
}
