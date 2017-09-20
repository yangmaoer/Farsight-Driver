#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>

char *gpx2data ;

int my_init(void)
{
	printk("init\n");

	gpx2data = ioremap(0x11000c44, 4);

	iowrite32(0xff, gpx2data);
	
	return 0;
}

void my_exit(void)
{
	iowrite32(0, gpx2data);

	iounmap(gpx2data);

	printk("exit\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur");

