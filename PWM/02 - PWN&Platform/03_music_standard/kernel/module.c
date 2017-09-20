#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define PWM 'l'
#define PWM_1_ON _IOR(PWM, 1, int)
#define PWM_1_OFF _IOR(PWM, 2, int)
#define PWM_1_MUSIC _IOR(PWM, 3, int)

typedef unsigned int u32;

struct PWM_cdev{
	dev_t PWM_cdev_num;
	struct cdev *PWM_cdev;
	u32 *GPD0CON;					//0x114000A0
	u32 *PWM_register;				//0x139D0000
}PWM_dev;

void io_wr32(u32 *addr, u32 and, u32 or)
{
	u32 data = ioread32(addr);
	data = (data & ~(and)) | (or);
	iowrite32(data, addr);	
}

//TCFG0 TCFG1 TCON TCNTB0 TCMPB0
void PWM_REGISTER_INIT(void)
{
	PWM_dev.GPD0CON 	 = ioremap(0x114000A0, 4);
	PWM_dev.PWM_register = ioremap(0x139D0000, 20);

	io_wr32(PWM_dev.GPD0CON,           0xf  << 0, 0x2 << 0);
	io_wr32(PWM_dev.PWM_register,      0xff << 0, 99);		//TCFG0
	io_wr32(PWM_dev.PWM_register + 1,  0xF,       0);		//TCFG1
	io_wr32(PWM_dev.PWM_register + 3,  ~0,        1000);	//TCNTB0
	io_wr32(PWM_dev.PWM_register + 4,  ~0,        500);		//TCMPB0
	io_wr32(PWM_dev.PWM_register + 2,  ~1,        0x2);		//TCON
	io_wr32(PWM_dev.PWM_register + 2,  0x1 << 1,  0x1<<3);	//TCON
}

void PWM_REGISTERR_EXIT(void)
{
	iounmap(PWM_dev.GPD0CON);
	iounmap(PWM_dev.PWM_register);
}

int PWM_open(struct inode *p, struct file *fp)
{
	printk("PWM_cdev_open\n");
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
	u32 k = 0;
	switch(cmd)
	{
	case PWM_1_OFF:
		io_wr32(PWM_dev.PWM_register + 2, 1, 0);
		break;
	case PWM_1_ON:
		io_wr32(PWM_dev.PWM_register + 2, 1, 1);
		break;
	case PWM_1_MUSIC:
		copy_from_user(&k, (u32 *)arg, sizeof k);
		if (k == 0)
		{
			io_wr32(PWM_dev.PWM_register + 2, 1, 0);
		}
		else
		{
			io_wr32(PWM_dev.PWM_register + 3, ~0, k);
			io_wr32(PWM_dev.PWM_register + 4, ~0, k/2);
			io_wr32(PWM_dev.PWM_register + 2, ~1, 0x3<<0);
			io_wr32(PWM_dev.PWM_register + 2, 1 << 1, 1 << 3);
			io_wr32(PWM_dev.PWM_register + 2, 1, 1);
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
	PWM_REGISTER_INIT();

	return 0;

add_failed:
	cdev_del(PWM_dev.PWM_cdev);
	unregister_chrdev_region(PWM_dev.PWM_cdev_num, 2);
	return ret;
}

void remove_PWM_cdev(void)
{
	PWM_REGISTERR_EXIT();
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
