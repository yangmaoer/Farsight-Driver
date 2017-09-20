#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/platform_device.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur@farsight.com.cn");
MODULE_DESCRIPTION("for linux driver learn");

struct platform_device *myplatformdev;

struct resource myresource[3] = {
	[0] = {
		.start = 0x11000c20,
		.end = 0x11000c20+8,
		.flags = IORESOURCE_MEM
	},
	[1] = {
		.start = 0,
		.name = "mypin"
	},
};

static int mymodule_init(void)
{
	
	myplatformdev = platform_device_alloc("xxx", -1);

	platform_device_add_resources(myplatformdev, myresource, 2);

	platform_device_add(myplatformdev);

	printk("module install\n");

	return 0;
}

static void mymodule_exit(void)
{
	printk("module release\n");
	platform_device_del(myplatformdev);
}


module_init(mymodule_init);
module_exit(mymodule_exit);
