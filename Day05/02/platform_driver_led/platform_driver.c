#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/ioctl.h>
#include <linux/mm.h>
#include <asm/io.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("for linux driver learn");

#define LED_ON _IOW('L', 0, int)
#define LED_OFF _IOW('L', 1, int)


struct platform_driver myplatformdriver = {0};

typedef unsigned int u32;

struct mycdev{
	dev_t devnum;
	struct cdev *cdev;

	u32 *ledbase;
	int pin;

	struct class *mytest_class;
	struct device *mytest_device;

}mydev;

int myopen(struct inode *inodep, struct file *filep)
{
	//ioremap
	//mydev.ledbase = ioremap(resource->start, 8);

	//init pin , output
	iowrite32( (ioread32(mydev.ledbase)&~(0xf<<mydev.pin*4)) | 1<<mydev.pin*4, mydev.ledbase);

	return 0;
}

ssize_t myread(struct file *filep, char __user *ubufer, size_t size, loff_t *offset)
{
	return 0;
}

ssize_t mywrite(struct file *filep, const char __user *ubufer, size_t size, loff_t *offset)
{
	return 0;
}

int myclose(struct inode *inodep, struct file *filep)
{
	return 0;
}


long myunlocked_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
	case LED_ON:
		iowrite32( ioread32(mydev.ledbase+1) | 1<<mydev.pin, mydev.ledbase+1);
		break;
	case LED_OFF:
		iowrite32( ioread32(mydev.ledbase+1) & ~(1<<mydev.pin), mydev.ledbase+1);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

int mymmap(struct file *filep, struct vm_area_struct *vma)
{

	return 0;
}

struct file_operations mycdev_ops = {
	.owner = THIS_MODULE,
	.open = myopen,
	.read = myread,
	.write = mywrite,
	.unlocked_ioctl = myunlocked_ioctl,
	.mmap = mymmap,
	.release = myclose,
};

int myprobe(struct platform_device *platformdev)
{
	struct resource *myresource_mem, *myresource_pin;

	int ret;
	printk("matched !\n");

	myresource_mem = platform_get_resource(platformdev, IORESOURCE_MEM, 0);//get gpcon addr
	if(NULL == myresource_mem)
	{
		printk("nomem\n");
	}
	else
		mydev.ledbase = ioremap(myresource_mem->start,  myresource_mem->end - myresource_mem->start);


	myresource_pin = platform_get_resource_byname(platformdev, 0,"mypin");//get led pin
	if(NULL != myresource_pin)
	{
		mydev.pin = myresource_pin->start;
	}
	//cdev add

	ret = alloc_chrdev_region(&mydev.devnum, 0, 1, "mycdev_test");
	if(ret < 0)
	{
		printk("devnum alloc failed !\n");
		return ret;
	}

	printk("num: %d\n", MAJOR(mydev.devnum) );

	mydev.cdev = cdev_alloc();
	mydev.cdev->ops  = &mycdev_ops;
	mydev.cdev->owner = THIS_MODULE;

	ret = cdev_add(mydev.cdev, mydev.devnum, 1);

	//time to create device nodfile!!!

	mydev.mytest_class = class_create(THIS_MODULE, "mytest");
	mydev.mytest_device = device_create(mydev.mytest_class, NULL,  mydev.devnum, NULL, "led%d", 0);

	return 0;

}


int myremove(struct platform_device *platformdev)
{
	printk("dev removed !\n");
	//time to delete device nodefile
	device_del(mydev.mytest_device);
	class_destroy(mydev.mytest_class);
	cdev_del(mydev.cdev);
	unregister_chrdev_region(mydev.devnum, 1);

	return 0;
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
