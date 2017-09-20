#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

struct cdev *XXX_cdev = cdev_alloc();
dev_t XXX_cdev_num;

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
	.open = XXX_open,
	.read = XXX_read,
	.write = XXX_write,
	.unlocked_ioctl = XXX_ioctl,
	.release = XXX_close,
};

int XXX_init(void)
{
	printk("XXX_cdev_init\n");

	//int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
	alloc_chrdev_region(&XXX_cdev_num, 0, 2, "XXX");
	printk("[MAJOR] : %d\n", MAJOR(XXX_cdev_num));
	printk("[MAJOR] : %d\n", MINOR(XXX_cdev_num));

	//void cdev_init(struct cdev *cdev, struct file_operations *fops);
	cdev_init(XXX_cdev, &XXX_fops);

	//int cdev_add(struct cdev *, dev_t, unsigned);
	cdev_add(XXX_cdev, XXX_cdev_num, 2);

	return 0;
}

void XXX_exit(void)
{
	printk("XXX_cdev_exit\n");

	//void cdev_del(struct cdev *);
	cdev_del(XXX_cdev);

	//void unregister_chrdev_region(dev_t from, unsigned count);
	unregister_chrdev_region(XXX_cdev_num, 2);
}
module_init(XXX_init);
module_exit(XXX_exit);

MODULE_AUTHOR("Tyrion Matrix");
MODULE_LICENSE("Dual BSD/GPL");
