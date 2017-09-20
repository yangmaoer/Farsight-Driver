#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	char buf[10];

	int flag = fcntl(0, F_GETFD);
	fcntl(0, F_SETFD, flag | O_NONBLOCK);
	gets(buf);// read(0, buf,);

	printf("end !\n");
}
