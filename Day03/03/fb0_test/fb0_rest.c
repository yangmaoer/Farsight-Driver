#include <linux/videodev2.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/fb.h>

#include <stdio.h>  
#include <strings.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
//#include <linux/in.h>  
#include <stdlib.h>  
#include <memory.h>  
#include <arpa/inet.h>  
#include <netinet/in.h>  
  
typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;

int main(int argc, char **argv)
{
	/*open fb device*/
	int fb = open("/dev/fb0", O_RDWR);
	if (fb < 0) {
		printf("Error: cannot open framebuffer device.\n");
		exit(1);
	}

	// Map the device to memory
	u8 *fbp = (u8 *)mmap(0, 10000000, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
	if (NULL == fbp) {
		printf("Error: failed to map framebuffer device to memory.\n");
		exit(4);
	}

	while(1)
	{
		memset(fbp, rand(), 10000000);
	//	printf("aaa\n");
	}

	return 0;
}
