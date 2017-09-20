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
	atomic_t avilable;	//原子变量
	QUE *Q;
}mydev;

int myopen(struct inode *inodep, struct file *filep)
{
	printk("open: %d\n", iminor(inodep));

	//对原子变量执行自减、并测试是否为0、如果为0、返回true
	if(0 == atomic_dec_and_test(&mydev.avilable) )
		return 0;
	else
	{
		//原子量不为0、要把减去的原子量加回去
		atomic_inc(&mydev.avilable);
		return -EBUSY;
	}
}

ssize_t myread(struct file *filep, char __user *ubufer, size_t size, loff_t *offset)
{
	ssize_t totalsize;
	char buf[512];	

	totalsize = (ssize_t)(get_que_num(mydev.Q) < size ? get_que_num(mydev.Q) : size);
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
	size = 512 > size ? size : 512;

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
	//退出的时候、回还原子量
	atomic_inc(&mydev.avilable);

	return 0;
}

long myunlocked_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	printk("mymmap\n");
	return 0;
}

//只要用户调用mmap、内核就会创建一个空白的vma结构体、传递给驱动的mmap方法
int mymmap(struct file *filep, struct vm_area_struct *vma)
{
	//int remap_pfn_range(struct vm_area_struct *vma, unsigned long from,
		   // unsigned long pfn, unsigned long size, pgprot_t prot)
	//remap_pfn_range() - 为一段物理内存创建新的页表
	//	-->为addr ~ addr+size 的虚拟地址构造页表
	if( remap_pfn_range(
			vma,	
			vma->vm_start,	//内存映射开始处的虚拟地址
			virt_to_phys(mydev.Q->data) >> PAGE_SHIFT,//key code
				//虚拟地址应该映射到的物理地址的页帧号
				//页帧号 - 实际上就是物理地址右移PAGE_SHIFT位
				//virt_to_phys() - 虚拟地址转换成物理地址
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
	//设置原子量的初值
	atomic_set(&mydev.avilable ,1);

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
