#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/platform_device.h>

struct platform_device *myplatformdev;

struct resource myresource[2] = {
	[0] = {
		.start = 0x139D0000,
		.end   = 0x139D0000 + 20,
		.flags = IORESOURCE_MEM
	},
	
	[1] = {
		.start = 0x114000A0,
		.end   = 0x114000A0 + 4,
		.flags = IORESOURCE_MEM
	}
};

static int mymodule_init(void)
{
	
	myplatformdev = platform_device_alloc("<XXX>", -1);

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

MODULE_AUTHOR("Tyrion Matrix");
MODULE_LICENSE("Dual BSD/GPL");
