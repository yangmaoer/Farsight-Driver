#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/device.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur");

int myfunc(struct device * devp,void * data)
{
	struct platform_device *platdev = container_of(devp, struct platform_device, dev);

	printk("%s\n", platdev->name);
	return 0;
}

int my_init(void)
{
	printk("init\n");
	bus_for_each_dev(&platform_bus_type, NULL, NULL, myfunc);
	
	return 0;
}
void my_exit(void)
{
	printk("exit\n");
}
module_init(my_init);
module_exit(my_exit);
