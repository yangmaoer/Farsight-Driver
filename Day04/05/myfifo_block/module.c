#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include "QUE/que.h"
#include <asm/uaccess.h>
#include <asm/ioctl.h>
#include <linux/mm.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur@farsight.com.cn");
MODULE_DESCRIPTION("for linux driver learn");


struct mycdev{
	dev_t devnum;
	struct cdev *cdev;
	// 睡眠队列 - 记录睡眠的进程
	wait_queue_head_t waitread_que;
	QUE *Q;
}mydev;

int myopen(struct inode *inodep, struct file *filep)
{
	printk("open: %d\n", iminor(inodep));

	return 0;
}

ssize_t myread(struct file *filep, char __user *ubufer, size_t size, loff_t *offset)
{
	ssize_t totalsize;
	char buf[512];

	// 如果不满足条件、就去睡眠
	// if no data then block current
	wait_event(mydev.waitread_que, 0 != get_que_num(mydev.Q) );

	totalsize = (ssize_t)(get_que_num(mydev.Q)<size?get_que_num(mydev.Q):size);
	deque(mydev.Q, buf, totalsize);

	if(copy_to_user(ubufer, buf, totalsize))
	{
		printk("read error\n");
		return -EIO;
	}

	return totalsize;
}

ssize_t mywrite(struct file *filep, const char __user *ubufer, size_t size, loff_t *offset)
{
	ssize_t totalsize;
	char buf[512];
	size = 512>size?size:512;

	if(copy_from_user(buf, ubufer, size))
	{
		printk("write error\n");
		return -EIO;
	}

	totalsize = enque(mydev.Q, buf, size);

	// 唤醒等待队列
	wake_up(&mydev.waitread_que);

	return totalsize;
}

int myclose(struct inode *inodep, struct file *filep)
{
	return 0;
}

long myunlocked_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	printk("mymmap\n");
	return 0;
}

int mymmap(struct file *filep, struct vm_area_struct *vma)
{
	if( remap_pfn_range(
			vma,
			vma->vm_start,
			vma->vm_pgoff + (virt_to_phys(mydev.Q->data) >> PAGE_SHIFT),//key code
			vma->vm_end - vma->vm_start,
			vma->vm_page_prot) )
	{
		printk("nomem\n");
		return -ENOMEM;
	}
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
	mydev.Q = create_que();

	// 初始化睡眠队列
	// 该队列设备私有、进程共享
	init_waitqueue_head(&mydev.waitread_que);

	ret = alloc_chrdev_region(&mydev.devnum, 0, 1, "mycdev_test");
	if(ret < 0)
	{
		printk("devnum alloc failed !\n");
		return ret;
	}

	printk("devnum: %d\n", MAJOR(mydev.devnum));

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
	destroy_que(mydev.Q);
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
