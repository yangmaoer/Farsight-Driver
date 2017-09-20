#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define LED 'l'
#define LED_1_ON _IOR(LED, 11, int)
#define LED_1_OFF _IOR(LED, 10, int)

struct led_gpio{
	char *gpx2data;
}led_data;

struct led_cdev{
	dev_t led_cdev_num;
	struct cdev *led_cdev;
}led_dev;


int led_open(struct inode *p, struct file *fp)
{
	printk("led_cdev_open\n");

	return 0;
}

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
	switch(cmd)
	{
		case LED_1_ON:
			//....
			led_data.gpx2data = ioremap(0x11000c44, 4);
			iowrite32(0xff, led_data.gpx2data);
			break;
		case LED_1_OFF:
			//....
			iowrite32(0, led_data.gpx2data);
			iounmap(led_data.gpx2data);
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

void remove_led_cdev(void)
{
	cdev_del(led_dev.led_cdev);
	unregister_chrdev_region(led_dev.led_cdev_num, 2);
}

static int led_init(void)
{
	printk("led_cdev_init\n");

	return setup_led_cdev();
}

void led_exit(void)
{
	remove_led_cdev();

	printk("led_cdev_exit\n");
}
module_init(led_init);
module_exit(led_exit);

MODULE_AUTHOR("Tyrion Matrix");
MODULE_LICENSE("Dual BSD/GPL");
