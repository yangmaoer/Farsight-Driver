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

struct RTC_time_data{
	u32 BCDYEAR;
	u32 BCDMON;
	u32 BCDWEEK;
	u32 BCDDAY;
	u32 BCDHOUR;
	u32 BCDMIN;
	u32 BCDSEC;
}tData;

hex2dec(struct RTC_time_data *tData)
{
	int unit, dec, hnd;
	unit = tData->BCDSEC % 16;
	dec = (tData->BCDSEC - unit) / 16;
	tData->BCDSEC = unit + dec * 10;
	
	unit = tData->BCDMIN % 16;
	dec = (tData->BCDMIN - unit) / 16;
	tData->BCDMIN = unit + dec * 10;
	
	unit = tData->BCDHOUR % 16;
	dec = (tData->BCDHOUR - unit) / 16;
	tData->BCDHOUR = unit + dec * 10;
	
	unit = tData->BCDWEEK % 16;
	tData->BCDWEEK = unit;
	
	unit = tData->BCDDAY % 16;
	dec = (tData->BCDDAY - unit) / 16;
	tData->BCDDAY = unit + dec * 10;
//		printf("<%d> ", tData->BCDDAY);
		
	unit = tData->BCDMON % 16;
	dec = (tData->BCDMON - unit) / 16;
	tData->BCDMON = unit + dec * 10;
	
	unit = tData->BCDYEAR % 16;
	dec = (tData->BCDYEAR - unit) / 16 % 16;
	hnd = ((tData->BCDYEAR - unit) / 16 - dec) / 16;
	tData->BCDYEAR = unit + dec * 10 + hnd * 100;
}

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
		int t = 15;
		printf("CMD>>>");
		scanf("%d",&cmd);
		getchar();
		switch (cmd)
		{
		case 0:
			while(t--)
			{
				ioctl(fd, RTC_DISPLAY, &tData);	
				hex2dec(&tData);
				printf("%d-%d-%d %d %d:%d:%d\n", 
					tData.BCDYEAR, 
					tData.BCDMON,
					tData.BCDDAY,
					tData.BCDWEEK,
					tData.BCDHOUR,
					tData.BCDMIN,
					tData.BCDSEC);
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
