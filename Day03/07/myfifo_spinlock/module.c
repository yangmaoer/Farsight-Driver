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
#include <linux/semaphore.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur@farsight.com.cn");
MODULE_DESCRIPTION("for linux driver learn");


struct mycdev{
	dev_t devnum;
	struct cdev *cdev;
	//定义自旋锁
	spinlock_t  lock;
	int avilable;
	QUE *Q;
}mydev;


/**********************************
**	自旋锁	-	可以用于中断
**		效率   -   高于信号量、一直在等待、不用唤醒了
**********************************/
int myopen(struct inode *inodep, struct file *filep)
{
	
	printk("open: %d\n", iminor(inodep));

	//获得自旋锁
	//	-	如果能够立即获得锁、它就马上返回
	//	-	如果不能、就在那里自旋等待
	spin_lock(&mydev.lock);	

	if(1 == mydev.avilable)
	{
		mydev.avilable = 0;

		spin_unlock(&mydev.lock);	
		return 0;
	}
	else
	{
		//释放自旋锁
		spin_unlock(&mydev.lock);	
		return -EBUSY;
	}
	return 0;
}

ssize_t myread(struct file *filep, char __user *ubufer, size_t size, loff_t *offset)
{
	ssize_t totalsize;
	char buf[512];	

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
	return totalsize;
}

int myclose(struct inode *inodep, struct file *filep)
{
	mydev.avilable = 1;

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
			virt_to_phys(mydev.Q->data) >> PAGE_SHIFT,//key code
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
	mydev.avilable = 1;

	//初始化自旋锁
	spin_lock_init(&mydev.lock);	

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
