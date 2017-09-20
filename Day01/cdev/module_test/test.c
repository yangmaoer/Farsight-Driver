#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur");



struct cdev mycdev;
dev_t mynum;

/*
struct file_operations myops = {0};
myops.open = myopen;
myops.close =  myclose;
*/

ssize_t myread(struct file *fp, char __user *buf, size_t size, loff_t *offset)
{
	//*buf = 'X';
	copy_to_user(buf, "hello", 5);
	return 5;
}

ssize_t mywrite(struct file *fp, const char __user *buf, size_t size, loff_t *offset)
{
	//.....
}

//int (*open) (struct inode *, struct file *);
int myopen(struct inode *p, struct file *fp)
{
	printk("open !!!\n");
	return 0;
}

int myclose(struct inode *p, struct file *fp)
{
	printk("close !!!\n");
	return 0;
}

struct file_operations myops = {
	.open = myopen,
	.read = myread,
	.write = mywrite,
	.release = myclose,
};

int __init my_init(void)
{
	printk("init\n");

	//request a avilable devnum !
	//int alloc_chrdev_region(dev_t *dev, unsigned int firstminor, unsigned int count, char *name) 
	alloc_chrdev_region(&mynum, 0, 2, "xxxx");	
	printk("devnum: %d\n", MAJOR(mynum));

	//init mycdev(fops)
	//void cdev_init(struct cdev *dev, struct file_operations *fops);
	cdev_init(&mycdev, &myops);

	//add mycdev into kernel
	//int cdev_add(struct cdev *dev, dev_t num, unsigned int count);
	cdev_add(&mycdev, mynum, 2);


	return 0;
}
void my_exit(void)
{
	printk("exit\n");
	
	cdev_del(&mycdev);
	unregister_chrdev_region(mynum, 2);
}
module_init(my_init);
module_exit(my_exit);
