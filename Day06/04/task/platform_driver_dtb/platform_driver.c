#include <linux/init.h>
#include <linux/time.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur@farsight.com.cn");
MODULE_DESCRIPTION("for linux driver learn");

struct platform_driver myplatformdriver = {0};

void working(unsigned long data)
{
	mdelay(1000);//work-simulation
	printk("done \n");
}


irqreturn_t myhandler(int num, void *data)
{
	printk("key !\n");

//	working();
	struct tasklet_struct *mytask;
	mytask = kmalloc(sizeof(struct tasklet_struct), GFP_ATOMIC);
	tasklet_init(mytask, working, 0);
	tasklet_schedule(mytask);

	return IRQ_HANDLED;
}

	struct resource *myresource_mem;
	struct resource *myresource_irq;
//	struct resource *myresource_pin;
int myprobe(struct platform_device *platformdev)
{
	int a;
	int pin;

	printk("matched !\n");
	myresource_mem = platform_get_resource(platformdev, IORESOURCE_MEM, 0);
	myresource_irq = platform_get_resource(platformdev, IORESOURCE_IRQ, 0);
	if(NULL == myresource_irq)
	{
		printk("no irq res\n");
	}
//	myresource_pin = platform_get_resource_byname(platformdev, 0, "pin");

	printk("MEM: [%#x  %#x]\n", myresource_mem->start, myresource_mem->end);
	//printk("IRQ: [%#x  %#x]\n", myresource_irq->start, myresource_irq->end);
	//printk("pin: [%#x]\n", myresource_pin->start);

	of_property_read_u32(platformdev->dev.of_node, "aaa", &a);
	printk("aaa: %d\n", a);

	pin = of_get_gpio(platformdev->dev.of_node, 0);
	gpio_request(pin, "pin");
	gpio_direction_output(pin, 0);
	gpio_set_value(pin, 1);

	if( request_irq(myresource_irq->start, 
			myhandler,
			IRQF_DISABLED | (myresource_irq->flags & IRQF_TRIGGER_MASK),
			"test irq",
			NULL) )
	{
		;//...
	}


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
	return 0;
}

struct of_device_id my_dts_table[2] = {
	[0] = {
		.compatible = "xxx",
	},

};

static int mymodule_init(void)
{

	printk("module install\n");
	
	myplatformdriver.driver.name = "aaaaaaaaaaa";
//	myplatformdriver.platform_device_id;
	myplatformdriver.driver.of_match_table = my_dts_table;
	
	myplatformdriver.driver.owner = THIS_MODULE;
	myplatformdriver.probe = myprobe;
	myplatformdriver.remove = myremove;

	platform_driver_register(&myplatformdriver);

	return 0;
}

static void mymodule_exit(void)
{
	printk("module release\n");
	free_irq(myresource_irq->start, NULL);
	platform_driver_unregister(&myplatformdriver);
}


module_init(mymodule_init);
module_exit(mymodule_exit);
