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
MODULE_AUTHOR("zhur@farsight.com.cn");
MODULE_DESCRIPTION("for linux driver learn");

#define LED_ON _IOW('L', 0, int)
#define LED_OFF _IOW('L', 1, int)


typedef unsigned int u32;

struct mycdev{
	dev_t devnum;
	struct cdev *cdev;

	u32 *gpx1base;
	u32 *gpx2base;
}mydev;

int myopen(struct inode *inodep, struct file *filep)
{
	//ioremap
	mydev.gpx1base = ioremap(0x11000c20, 8);
	mydev.gpx2base = ioremap(0x11000c40, 8);

	//init pin , output
	iowrite32( (ioread32(mydev.gpx1base)&~0xf) | 1, mydev.gpx1base);
	iowrite32( (ioread32(mydev.gpx2base)&~(0xf<<28)) | 1<<28, mydev.gpx2base);
	
	printk("open: %d\n", iminor(inodep));

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

void led_on(unsigned long num)
{
	switch(num)
	{
	case 1:
		iowrite32( ioread32(mydev.gpx1base+1) | 1, mydev.gpx1base+1);
		break;
	case 2:
		iowrite32( ioread32(mydev.gpx2base+1) | 1<<7, mydev.gpx2base+1);
		break;
			
	}
}

void led_off(unsigned long num)
{
	switch(num)
	{
	case 1:
		iowrite32( ioread32(mydev.gpx1base+1) & ~1, mydev.gpx1base+1);
		break;
	case 2:
		iowrite32( ioread32(mydev.gpx2base+1) & ~(1<<7), mydev.gpx2base+1);
		break;
	}
}

long myunlocked_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
	case LED_ON:
		led_on(arg);
		printk("on: %d\n", arg);
		break;
	case LED_OFF:
		led_off(arg);
		printk("off: %d\n", arg);
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

int setup_mydev(void)
{
	int ret;

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
	if(ret)
	{	
		printk("devnum alloc failed !\n");
		goto add_failed;
	}

	return 0;
	cdev_del(mydev.cdev);
add_failed:
	unregister_chrdev_region(mydev.devnum, 1);

	return ret;
}

void remove_mycdev(void)
{
	cdev_del(mydev.cdev);
	unregister_chrdev_region(mydev.devnum, 1);

}

static int mymodule_init(void)
{

	printk("module install\n");

	return setup_mydev();	
}

static void mymodule_exit(void)
{
	remove_mycdev();

	printk("module release\n");
}


module_init(mymodule_init);
module_exit(mymodule_exit);
