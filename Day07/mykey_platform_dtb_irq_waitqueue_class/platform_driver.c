#include <linux/init.h>
#include <linux/time.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h> 
#include <asm/ioctl.h>
#include <linux/mm.h>
#include <linux/wait.h>

/********************************************************
**				< 程序思路 >
**  用户	-	决定策略(应用程序决定驱动实现什么功能)
**	驱动	-	实现机制（向<关注key驱动的>用户层<程序>提供信息）
**
**	APP	-	read(key)
**			switch(key_cmd)			
**			case ..	-->	ioctl
**
**		切记	-	驱动只是实现机制、向用户传递信息
**				-	不用去自觉主张的实现什么功能	-	要问用户需不需要
********************************************************/

struct mycdev{
	dev_t devnum;
	struct cdev *cdev;

	//要保证唤醒的那个就是睡眠的那个	-	睡眠对列
	wait_queue_head_t readque;
	int irqnum;
	//不应该是一来read就被睡眠	-	而是在没有按键按下的情况下才去睡眠
	int keyflag;	
	int keycode;	//用于区分是哪个按键
}mydev;

int myopen(struct inode *inodep, struct file *filep)
{
	printk("open: %d\n", iminor(inodep));

	return 0;
}

//唤醒	-	由中断处理函数完成
irqreturn_t myhandler(int num, void *data)
{
	static unsigned long last = 0;

	//判断两次下降沿之间的时间	-	初级消抖
	if(jiffies - last > 50)
	{
		mydev.keycode = 'X';
		mydev.keyflag = 1;	//中断处理函数中把flag置一

		wake_up_interruptible(&mydev.readque);	
		//跟新last
		last = jiffies;
	}

        return IRQ_HANDLED;
}

//read阻塞		
//【1】	-	没有数据 -> 阻塞对列
//【2】 -	有数据了 -> 中断处理函数负责唤醒
ssize_t myread(struct file *filep, char __user *ubufer, size_t size, loff_t *offset)
{
	//read	-	阻塞对列
	//并判断按键标志位
	//当条件不满足	-	睡眠
	wait_event_interruptible(mydev.readque, 0 != mydev.keyflag);

	put_user(mydev.keycode, ubufer);

	mydev.keyflag = 0;		//read后恢复按键标志位置零

	return 1;
}

ssize_t mywrite(struct file *filep, const char __user *ubufer, size_t size, loff_t *offset)
{
	return 0;
}

int myclose(struct inode *inodep, struct file *filep)
{
	return 0;
}

long myunlocked_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	return 0;
}

int mymmap(struct file *filep, struct vm_area_struct *vma)
{
	
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

	ret = alloc_chrdev_region(&mydev.devnum, 0, 1, "mycdev_test");
	if(ret < 0)
	{
		printk("devnum alloc failed !\n");
		return ret; 
	}
	printk("num: %d\n", MAJOR(mydev.devnum) );
	
	init_waitqueue_head(&mydev.readque);
	//按键标志位初始化
	mydev.keyflag = 0;

	//if	-	返回值判断
	mydev.cdev = cdev_alloc(); 
	mydev.cdev->ops  = &mycdev_ops;
	mydev.cdev->owner = THIS_MODULE;

	ret = cdev_add(mydev.cdev, mydev.devnum, 1);
	if(ret)
	{	
		printk("devnum alloc failed !\n");
		goto add_failed;
	}

	//中断算设备的一部分	-	应该在设备初始化的时候申请中断
	//mydev.irqnum	-	？？？？？
	//NULL	-	不依附任何设备
	//如果是&mydev.cde->dev呢	-	？？？？？
	if( request_irq(mydev.irqnum, 
			myhandler,
			IRQF_DISABLED | IRQF_TRIGGER_FALLING,
			"test irq",
			NULL) )
	{
		;//...
	}

	return 0;
//goto？？	-	栈式	
	cdev_del(mydev.cdev);
add_failed:
	unregister_chrdev_region(mydev.devnum, 1);

	return ret;
}

void remove_mycdev(void)
{
	cdev_del(mydev.cdev);
	unregister_chrdev_region(mydev.devnum, 4);
}

struct platform_driver myplatformdriver = {0};

struct resource *myresource_irq;

int myprobe(struct platform_device *platformdev)
{
	printk("matched !\n");
	//获取中断资源
	myresource_irq = platform_get_resource(platformdev, IORESOURCE_IRQ, 0);

	mydev.irqnum = myresource_irq->start;

	//初始化设备
	setup_mydev();

/*	
	cdev_add();
	...
	...
*/
	return 0;
}

int myremove(struct platform_device *platformdev)
{
	printk("dev removed !\n");
	free_irq(mydev.irqnum, NULL);
	remove_mycdev();
	return 0;
}

struct of_device_id my_dts_table[2] = {
	[0] = {
		.compatible = "xxx",
	},
};

static int mymodule_init(void)
{
	printk("module install\n");
	
	myplatformdriver.driver.name = "aaaaaaaaaaa";
	myplatformdriver.driver.of_match_table = my_dts_table;
	
	myplatformdriver.driver.owner = THIS_MODULE;
	myplatformdriver.probe = myprobe;
	myplatformdriver.remove = myremove;

	platform_driver_register(&myplatformdriver);

	return 0;
}

static void mymodule_exit(void)
{
	printk("module release\n");
	//中断移除
	free_irq(myresource_irq->start, NULL);
	platform_driver_unregister(&myplatformdriver);
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("Dual BSD/GPL");