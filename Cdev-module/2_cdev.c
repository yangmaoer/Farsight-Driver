#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

struct XXX_cdev{
	dev_t XXX_cdev_num;
	struct cdev *XXX_cdev;

}XXX_dev;


int XXX_open(struct inode *p, struct file *fp)
{
	printk("XXX_cdev_open\n");

	return 0;
}

ssize_t XXX_write(struct file *fp, const char __user *buf, size_t size, loff_t *offset)
{
	//copy_from_user(void *to, count void __user *from, unsigned long count);

	return 0;
}

ssize_t XXX_read(struct file *fp, char __user *buf, size_t size, loff_t *offset)
{
	//copy_to_user(void __user *to, const void *from, unsigned long count);
	
	return 0;
}

long XXX_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{


	return 0;
}

int XXX_close(struct inode *p, struct file *fp)
{
	printk("XXX_cdev_close\n");

	return 0;
}

struct file_operations XXX_fops = {
	.owner = THIS_MODULE,
	.open = XXX_open,
	.read = XXX_read,
	.write = XXX_write,
	.unlocked_ioctl = XXX_ioctl,
	.release = XXX_close,
};

int setup_XXX_cdev(void)
{
	int ret;

	//int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
	ret = alloc_chrdev_region(&XXX_dev.XXX_cdev_num, 0, 2, "XXX_test");
	if (ret < 0)
	{
		printk("XXX_cdev_num alloc failed!\n");
		return ret;
	}

	printk("[MAJOR] : %d\n", MAJOR(XXX_dev.XXX_cdev_num));
	printk("[MAJOR] : %d\n", MINOR(XXX_dev.XXX_cdev_num));
	
	XXX_dev.XXX_cdev = cdev_alloc();
    XXX_dev.XXX_cdev->ops = &XXX_fops;
	XXX_dev.XXX_cdev->owner = THIS_MODULE;

	//int cdev_add(struct cdev *, dev_t, unsigned);
	ret = cdev_add(XXX_dev.XXX_cdev, XXX_dev.XXX_cdev_num, 2);
	if (ret)
	{
		printk("XXX_cdev_num alloc failed!\n");
		goto add_failed;
	}

	return 0;

add_failed:
	cdev_del(XXX_dev.XXX_cdev);
	unregister_chrdev_region(XXX_dev.XXX_cdev_num, 2);
	return ret;
}

void remove_XXX_cdev(void)
{
	cdev_del(XXX_dev.XXX_cdev);
	unregister_chrdev_region(XXX_dev.XXX_cdev_num, 2);
}

static int XXX_init(void)
{
	printk("XXX_cdev_init\n");

	return setup_XXX_cdev();
}

void XXX_exit(void)
{
	remove_XXX_cdev();

	printk("XXX_cdev_exit\n");
}
module_init(XXX_init);
module_exit(XXX_exit);

MODULE_AUTHOR("Tyrion Matrix");
MODULE_LICENSE("Dual BSD/GPL");
