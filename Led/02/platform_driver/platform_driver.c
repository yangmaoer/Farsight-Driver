#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/platform_device.h>

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define LED 'l'
#define LED_1_ON _IOR(LED, 11, int)
#define LED_1_OFF _IOR(LED, 10, int)

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur@farsight.com.cn");
MODULE_DESCRIPTION("for linux driver learn");

struct platform_driver myplatformdriver = {0};

int led_open(struct inode *p, struct file *fp)
{
	printk("led_cdev_open\n");

	return 0;
}

struct led_register{
	char *gpx2con;
}led_data;

struct led_cdev{
	dev_t led_cdev_num;
	struct cdev *led_cdev;
}led_dev;

ssize_t led_write(struct file *fp, const char __user *buf, size_t size, loff_t *offset)
{
	//copy_from_user(void *to, count void __user *from, unsigned long count);

	return 0;
}

ssize_t led_read(struct file *fp, char __user *buf, size_t size, loff_t *offset)
{
	//copy_to_user(void __user *to, const void *from, unsigned long count);
	
	return 0;
}

long led_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	//myresource_mem->start, myresource_mem->end
	switch(cmd)
	{
		case LED_1_ON:
			//....
			iowrite32(0xff, led_data.gpx2data+1);
			break;
		case LED_1_OFF:
			//....
			iowrite32(0, led_data.gpx2data+1);
			break;
	}

	return 0;
}

int led_close(struct inode *p, struct file *fp)
{
	printk("led_cdev_close\n");

	return 0;
}
struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.open = led_open,
	.read = led_read,
	.write = led_write,
	.unlocked_ioctl = led_ioctl,
	.release = led_close,
};

int setup_led_cdev(void)
{
	int ret;

	//int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
	ret = alloc_chrdev_region(&led_dev.led_cdev_num, 0, 2, "led_test");
	if (ret < 0)
	{
		printk("led_cdev_num alloc failed!\n");
		return ret;
	}

	printk("[MAJOR] : %d\n", MAJOR(led_dev.led_cdev_num));
	printk("[MAJOR] : %d\n", MINOR(led_dev.led_cdev_num));
	
	led_dev.led_cdev = cdev_alloc();
    led_dev.led_cdev->ops = &led_fops;
	led_dev.led_cdev->owner = THIS_MODULE;

	//int cdev_add(struct cdev *, dev_t, unsigned);
	ret = cdev_add(led_dev.led_cdev, led_dev.led_cdev_num, 2);
	if (ret)
	{
		printk("led_cdev_num alloc failed!\n");
		goto add_failed;
	}

	return 0;

add_failed:
	cdev_del(led_dev.led_cdev);
	unregister_chrdev_region(led_dev.led_cdev_num, 2);
	return ret;
}


int myprobe(struct platform_device *platformdev)
{
	struct resource *myresource_mem;
	struct resource *myresource_irq;
	printk("matched !\n");

	myresource_mem = platform_get_resource(platformdev, IORESOURCE_MEM, 0);
	myresource_irq = platform_get_resource(platformdev, IORESOURCE_IRQ, 0);

	printk("MEM: [%#x  %#x]\n", myresource_mem->start, myresource_mem->end);
	printk("IRQ: [%#x  %#x]\n", myresource_irq->start, myresource_irq->end);
/*	
	cdev_add();
	...
	...
*/
	setup_led_cdev();
	led_register.gpx2con = ioremap(myresource_mem->start, 4);
	

	return 0;

}


int myremove(struct platform_device *platformdev)
{
	printk("dev removed !\n");
	iounmap(led_register.gpx2con);
}


static int mymodule_init(void)
{

	printk("module install\n");
	
	myplatformdriver.driver.name = "xxx";
	myplatformdriver.driver.owner = THIS_MODULE;
	myplatformdriver.probe = myprobe;
	myplatformdriver.remove = myremove;

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
