#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/delay.h>

#define PWM 'l'
#define PWM_1_ON _IOR(PWM, 11, int)
#define PWM_1_OFF _IOR(PWM, 10, int)
#define PWM_1 _IOR(PWM, 12, int)

unsigned int volume[15] = {0, 1912, 1730, 1517, 1432, 1275, 1136, 1012, 965, 851, 758, 715, 637, 568, 506};
int t[27] = {6,6,8,7,6,5,6,3,0,6,5,6,6,8,7,8,6,7,8,0,8,9,10,10,10,10,9};

struct PWM_cdev{
	dev_t PWM_cdev_num;
	struct cdev *PWM_cdev;
}PWM_dev;

struct PWM_GPIO{
	char *gpd0con;
	char *TCFG0;
	char *TCFG1;
	char *TCON;
	char *TCNTB0;
	char *TCMPB0;
}pwm;

unsigned int data;
void music(int tunes)
{
	//	data = ioread32(pwm.gpd0con);
	//	iowrite32(data & ~(0xf) | 0x2, pwm.gpd0con)	;
	if (tunes == 0)
	{
		data = ioread32(pwm.TCON);
		iowrite32(data & ~1, pwm.TCON);
	}
	else
	{
		iowrite32(tunes, pwm.TCNTB0);
		iowrite32(tunes/2, pwm.TCMPB0);
		data = ioread32(pwm.TCON);
		iowrite32((data | 0x3), pwm.TCON);
		data = ioread32(pwm.TCON);
		iowrite32(data & ~(1<<1) | 1<<3, pwm.TCON);
		data = ioread32(pwm.TCON);
		iowrite32(data | ((1)), pwm.TCON);
	}
}


void PWM_GPIO_INIT(void)
{
	pwm.gpd0con = ioremap(0x114000A0, 4);
	pwm.TCFG0 = ioremap(0x139D0000, 4);
	pwm.TCFG1 = ioremap(0x139D0004, 4);
	pwm.TCON = ioremap(0x139D0008, 4);
	pwm.TCNTB0 = ioremap(0x139D000c, 4);
	pwm.TCMPB0 = ioremap(0x139D0010, 4);

	data = ioread32(pwm.gpd0con);
	iowrite32(((data & ~(0xf)) | 0x2), pwm.gpd0con)	;
	data = ioread32(pwm.TCFG0);
	iowrite32(((data & ~(0xff)) | 99), pwm.TCFG0);
	data = ioread32(pwm.TCFG1);
	iowrite32(data & ~(0xf), pwm.TCFG1);
	iowrite32(1000, pwm.TCNTB0);
	iowrite32(500, pwm.TCMPB0);
	data = ioread32(pwm.TCON);
	iowrite32((data | ( 3)), pwm.TCON);
	data = ioread32(pwm.TCON);
	iowrite32(data & ~(1<<1) | 1<<3, pwm.TCON);
	//		data = ioread32(pwm.TCON);
	//		iowrite32(data | ((1)), pwm.TCON);
}

int PWM_open(struct inode *p, struct file *fp)
{
	printk("PWM_cdev_open\n");

	PWM_GPIO_INIT();

	return 0;
}

ssize_t PWM_write(struct file *fp, const char __user *buf, size_t size, loff_t *offset)
{
	//copy_from_user(void *to, count void __user *from, unsigned long count);

	return 0;
}

ssize_t PWM_read(struct file *fp, char __user *buf, size_t size, loff_t *offset)
{
	//copy_to_user(void __user *to, const void *from, unsigned long count);

	return 0;
}

long PWM_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	unsigned int k = 0;
	switch(cmd)
	{
	case PWM_1_OFF:
		data = ioread32(pwm.TCON);
		iowrite32(data & (~(1)), pwm.TCON);
		break;
	case PWM_1_ON:

		data = ioread32(pwm.TCON);
		iowrite32(data | ((1)), pwm.TCON);
		//		data = ioread32(pwm.TCON);
		//		iowrite32((data | 0x3), pwm.TCON);
		//		data = ioread32(pwm.TCON);
		//		iowrite32(data & ~(1<<1) | 1<<3, pwm.TCON);
		break;
	case PWM_1:

		for (k = 0; k < 27; k++)
		{
			data = ioread32(pwm.TCON);
			iowrite32(data & (~(1)), pwm.TCON);
			printk("\t%d\n", k);
			music(volume[t[k]]);
			mdelay(500);
		}
		break;
	}

	return 0;
}

int PWM_close(struct inode *p, struct file *fp)
{
	printk("PWM_cdev_close\n");

	return 0;
}

struct file_operations PWM_fops = {
	.owner = THIS_MODULE,
	.open = PWM_open,
	.read = PWM_read,
	.write = PWM_write,
	.unlocked_ioctl = PWM_ioctl,
	.release = PWM_close,
};

int setup_PWM_cdev(void)
{
	int ret;

	//int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
	ret = alloc_chrdev_region(&PWM_dev.PWM_cdev_num, 0, 2, "PWM_test");
	if (ret < 0)
	{
		printk("PWM_cdev_num alloc failed!\n");
		return ret;
	}

	printk("[MAJOR] : %d\n", MAJOR(PWM_dev.PWM_cdev_num));
	printk("[MAJOR] : %d\n", MINOR(PWM_dev.PWM_cdev_num));

	PWM_dev.PWM_cdev = cdev_alloc();
	PWM_dev.PWM_cdev->ops = &PWM_fops;
	PWM_dev.PWM_cdev->owner = THIS_MODULE;

	//int cdev_add(struct cdev *, dev_t, unsigned);
	ret = cdev_add(PWM_dev.PWM_cdev, PWM_dev.PWM_cdev_num, 2);
	if (ret)
	{
		printk("PWM_cdev_num alloc failed!\n");
		goto add_failed;
	}

	return 0;

add_failed:
	cdev_del(PWM_dev.PWM_cdev);
	unregister_chrdev_region(PWM_dev.PWM_cdev_num, 2);
	return ret;
}

void remove_PWM_cdev(void)
{
	cdev_del(PWM_dev.PWM_cdev);
	unregister_chrdev_region(PWM_dev.PWM_cdev_num, 2);
}

static int PWM_init(void)
{
	printk("PWM_cdev_init\n");

	return setup_PWM_cdev();
}

void PWM_exit(void)
{
	remove_PWM_cdev();

	printk("PWM_cdev_exit\n");
}
module_init(PWM_init);
module_exit(PWM_exit);

MODULE_AUTHOR("Tyrion Matrix");
MODULE_LICENSE("Dual BSD/GPL");
