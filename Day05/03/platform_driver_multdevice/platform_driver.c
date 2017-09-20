#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/platform_device.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur@farsight.com.cn");
MODULE_DESCRIPTION("for linux driver learn");

struct platform_driver myplatformdriver = {0};


int myprobe(struct platform_device *platformdev)
{
	struct resource *myresource_mem;
	struct resource *myresource_irq;
	struct resource *myresource_pin;
	printk("matched !\n");

	myresource_mem = platform_get_resource(platformdev, IORESOURCE_MEM, 0);
	myresource_irq = platform_get_resource(platformdev, IORESOURCE_IRQ, 0);
	myresource_pin = platform_get_resource_byname(platformdev, 0, "pin");

	printk("MEM: [%#x  %#x]\n", myresource_mem->start, myresource_mem->end);
	printk("IRQ: [%#x  %#x]\n", myresource_irq->start, myresource_irq->end);
	printk("pin: [%#x]\n", myresource_pin->start);
/*	
	cdev_add();
	...
	...
*/
	return 0;

}


int myremove(struct platform_device *platformdev)
{
	printk("dev removed !\n");
}

struct platform_device_id idtable[] = {
	[0]={
		.name = "yyy",
	},

	[1]={
		.name = "kkk",
	}
};

static int mymodule_init(void)
{

	printk("module install\n");
	
	myplatformdriver.driver.name = "xxx";
	myplatformdriver.driver.owner = THIS_MODULE;
	myplatformdriver.probe = myprobe;
	myplatformdriver.remove = myremove;
	myplatformdriver.id_table = &idtable;

	platform_driver_register(&myplatformdriver);

	return 0;
}

static void mymodule_exit(void)
{
	printk("module release\n");
	platform_driver_unregister(&myplatformdriver);
}


module_init(mymodule_init);
module_exit(mymodule_exit);
