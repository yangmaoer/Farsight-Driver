#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define RTC 'R'
#define RTC_DISPLAY _IOR(RTC, 0, int)
#define RTC_SET     _IOW(RTC, 1, int)
#define RTC_UNDIS   _IOR(RTC, 2, int)

typedef unsigned int u32;

struct RTC_time_data{
	u32 BCDYEAR;
	u32 BCDMON;
	u32 BCDWEEK;
	u32 BCDDAY;
	u32 BCDHOUR;
	u32 BCDMIN;
	u32 BCDSEC;
}tData;

struct RTC_cdev{
	dev_t RTC_cdev_num;
	struct cdev *RTC_cdev;
	u32 *RTCCON;	
		//0x10070040
	u32 * BCD_time;	
		//SEC MIN HOUR DAYWEEK DAY MON YEAR 0X10070070
}RTC_dev;


void io_wr32(u32 *addr, u32 and, u32 or)
{
	u32 data = ioread32(addr);
	data = data & ~(and) | (or);
	iowrite32(data, addr);	
}

int RTC_open(struct inode *p, struct file *fp)
{
	printk("RTC_cdev_open\n");
	//ioremap
	RTC_dev.RTCCON = ioremap(0x10070040, 4);
	RTC_dev.BCD_time = ioremap(0X10070070, 28);
	
	//init pin, RTC_CONTROL
	//RTCCON = RTCCON | 1;
	io_wr32(RTC_dev.RTCCON, 1 << 0, 1 << 0);
	
	//init RTC time
			//SEC MIN HOUR DAYWEEK DAY MON YEAR 0X10070070
	io_wr32(RTC_dev.BCD_time + 0, ~0, 0x00);//SEC
	io_wr32(RTC_dev.BCD_time + 1, ~0, 0x11);//MIN
	io_wr32(RTC_dev.BCD_time + 2, ~0, 0x19);//HOUR
	io_wr32(RTC_dev.BCD_time + 3, ~0, 0x03);//DAYWEEK
	io_wr32(RTC_dev.BCD_time + 4, ~0, 0x31);//DAY
	io_wr32(RTC_dev.BCD_time + 5, ~0, 0x05);//MON
	io_wr32(RTC_dev.BCD_time + 6, ~0, 0x16);//YEAR
	
	//Close RTC_CONTROL
	//RTCCON = RTCCON & (~1);
	io_wr32(RTC_dev.RTCCON, 1 << 0, 0 << 0);
	
	return 0;
}

ssize_t RTC_write(struct file *fp, const char __user *buf, size_t size, loff_t *offset)
{
	//copy_from_user(void *to, count void __user *from, unsigned long count);

	return 0;
}

ssize_t RTC_read(struct file *fp, char __user *buf, size_t size, loff_t *offset)
{
	//copy_to_user(void __user *to, const void *from, unsigned long count);
	
	return 0;
}

long RTC_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
	case RTC_DISPLAY:
	//time - read
		tData.BCDSEC = ioread32(RTC_dev.BCD_time + 0);
		put_user(tData.BCDSEC, (u32 *)arg);
		break;
	case RTC_SET:

		break;
	case RTC_UNDIS:

		break;
	default:
		return -EINVAL;
	}
	return 0;
}

int RTC_close(struct inode *p, struct file *fp)
{
	printk("RTC_cdev_close\n");

	return 0;
}

struct file_operations RTC_fops = {
	.owner = THIS_MODULE,
	.open = RTC_open,
	.read = RTC_read,
	.write = RTC_write,
	.unlocked_ioctl = RTC_ioctl,
	.release = RTC_close,
};

int setup_RTC_cdev(void)
{
	int ret;

	//int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
	ret = alloc_chrdev_region(&RTC_dev.RTC_cdev_num, 0, 2, "RTC_test");
	if (ret < 0)
	{
		printk("RTC_cdev_num alloc failed!\n");
		return ret;
	}

	printk("[MAJOR] : %d\n", MAJOR(RTC_dev.RTC_cdev_num));
	printk("[MAJOR] : %d\n", MINOR(RTC_dev.RTC_cdev_num));
	
	RTC_dev.RTC_cdev = cdev_alloc();
    RTC_dev.RTC_cdev->ops = &RTC_fops;
	RTC_dev.RTC_cdev->owner = THIS_MODULE;

	//int cdev_add(struct cdev *, dev_t, unsigned);
	ret = cdev_add(RTC_dev.RTC_cdev, RTC_dev.RTC_cdev_num, 2);
	if (ret)
	{
		printk("RTC_cdev_num alloc failed!\n");
		goto add_failed;
	}

	return 0;

add_failed:
	cdev_del(RTC_dev.RTC_cdev);
	unregister_chrdev_region(RTC_dev.RTC_cdev_num, 2);
	return ret;
}

void remove_RTC_cdev(void)
{
	cdev_del(RTC_dev.RTC_cdev);
	unregister_chrdev_region(RTC_dev.RTC_cdev_num, 2);
}

static int RTC_init(void)
{
	printk("RTC_cdev_init\n");

	return setup_RTC_cdev();
}

void RTC_exit(void)
{
	remove_RTC_cdev();

	printk("RTC_cdev_exit\n");
}
module_init(RTC_init);
module_exit(RTC_exit);

MODULE_AUTHOR("Tyrion Matrix");
MODULE_LICENSE("Dual BSD/GPL");
