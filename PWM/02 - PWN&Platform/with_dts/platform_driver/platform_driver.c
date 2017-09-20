#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h> 
#include <asm/ioctl.h>
#include <asm/io.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#define PWM 'l'
#define PWM_1_ON _IOR(PWM, 1, int)
#define PWM_1_OFF _IOR(PWM, 2, int)
#define PWM_1_MUSIC _IOR(PWM, 3, int)

typedef unsigned int u32;

struct platform_driver myplatformdriver = {0};

struct mycdev{
	dev_t devnum;
	struct cdev *cdev;
	struct class *mytest_class;
	struct device *mytest_device;
	u32 *pwmbase;
	u32 *GPD0CON;
	int pin1;
}mydev;

void io_wr32(u32 *addr, u32 and, u32 or)
{
	u32 data = ioread32(addr);
	data = (data & ~(and)) | (or);
	iowrite32(data, addr);	
}

void PWM_REGISTER_INIT(void)
{
	io_wr32(mydev.GPD0CON,      0xf  << 0, 0x2 << 0);
	io_wr32(mydev.pwmbase,      0xff << 0, 99);			//TCFG0
	io_wr32(mydev.pwmbase + 1,  0xF,       0);			//TCFG1
	io_wr32(mydev.pwmbase + 3,  ~0,        1000);		//TCNTB0
	io_wr32(mydev.pwmbase + 4,  ~0,        500);		//TCMPB0
	io_wr32(mydev.pwmbase + 2,  ~1,        0x2);		//TCON
	io_wr32(mydev.pwmbase + 2,  0x1 << 1,  0x1<<3);		//TCON
}

int myopen(struct inode *inodep, struct file *filep)
{
	return 0;
}

ssize_t myread(struct file *filep, char __user *ubufer, size_t size, loff_t *offset)
{
	return 0;
}

ssize_t mywrite(struct file *filep, const char __user *ubufer, size_t size, loff_t *offset)
{
	return 0;
}

int myclose(struct inode *inodep, struct file *filep)
{
	printk("PWM_cdev_close\n");
	return 0;
}

long myunlocked_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	u32 k = 0;
	unsigned long ret_copy;
	switch(cmd)
	{
	case PWM_1_ON:	
		io_wr32(mydev.pwmbase + 2, 1, 1);	
		break;
		
	case PWM_1_OFF:
		io_wr32(mydev.pwmbase + 2, 1, 0);
		break;
		
	case PWM_1_MUSIC:
		ret_copy = copy_from_user(&k, (u32 *)arg, sizeof k);
		if (k == 0)
		{
			io_wr32(mydev.pwmbase + 2, 1, 0);
		}
		else
		{
			io_wr32(mydev.pwmbase + 3, ~0, k);
			io_wr32(mydev.pwmbase + 4, ~0, k/2);
			io_wr32(mydev.pwmbase + 2, ~1, 0x3<<0);
			io_wr32(mydev.pwmbase + 2, 1 << 1, 1 << 3);
			io_wr32(mydev.pwmbase + 2, 1, 1);
		}
		break;
		
	default:
		return -EINVAL;
	}
	return 0;
}


struct file_operations mycdev_ops = {
	.owner = THIS_MODULE,
	.open = myopen,
	.read = myread,
	.write = mywrite,
	.unlocked_ioctl = myunlocked_ioctl,
	.release = myclose,
};

irqreturn_t myhandler(int num, void *data)
{
	printk("key !\n");
	return IRQ_HANDLED;
}

int setup_PWM_cdev(struct platform_device *platformdev)
{
	int ret;
	struct resource *myresource_mem;
	struct resource *myresource_irq;
	
	myresource_mem = platform_get_resource(platformdev, IORESOURCE_MEM, 0);//get gpcon addr
	printk("MEM: [%#x  %#x]\n", myresource_mem->start, myresource_mem->end);
	mydev.pwmbase = ioremap(myresource_mem->start,  myresource_mem->end - myresource_mem->start);
	
	myresource_mem = platform_get_resource(platformdev, IORESOURCE_MEM, 1);
	printk("MEM: [%#x  %#x]\n", myresource_mem->start, myresource_mem->end);
	mydev.GPD0CON = ioremap(myresource_mem->start,  myresource_mem->end - myresource_mem->start);
	
	int pin2;
	
	mydev.pin1 = of_get_gpio(platformdev->dev.of_node, 0);
	gpio_request(mydev.pin1, "pin");
	gpio_direction_output(mydev.pin1, 0);
	gpio_set_value(mydev.pin1, 1);
	
	myresource_irq = platform_get_resource(platformdev, IORESOURCE_IRQ, 0);
	if( request_irq(myresource_irq->start, 
			myhandler,
			IRQF_DISABLED | (myresource_irq->flags & IRQF_TRIGGER_MASK),
			"test irq",
			NULL) )
	{
		;//...
	}
	//cdev add
	ret = alloc_chrdev_region(&mydev.devnum, 0, 1, "mycdev_test");
	if(ret < 0)
	{
		printk("devnum alloc failed !\n");
		return ret; 
	}
	
	printk("num: %d\n", MAJOR(mydev.devnum));

	mydev.cdev = cdev_alloc();
	mydev.cdev->ops  = &mycdev_ops;
	mydev.cdev->owner = THIS_MODULE;

	ret = cdev_add(mydev.cdev, mydev.devnum, 1);
	
	//time to create device nodfile!!!
	mydev.mytest_class = class_create(THIS_MODULE, "mytest");
	mydev.mytest_device = device_create(mydev.mytest_class, NULL,  mydev.devnum, NULL, "pwm%d", 0);

	PWM_REGISTER_INIT();
	
	return 0;
}

int myprobe(struct platform_device *platformdev)
{
	printk("matched !\n");
	
	setup_PWM_cdev(platformdev);

	return 0;
}


int myremove(struct platform_device *platformdev)
{
	printk("dev removed !\n");
	
	//time to delete device nodefile
	iounmap(mydev.GPD0CON);
	iounmap(mydev.pwmbase);
	
	device_del(mydev.mytest_device);
	class_destroy(mydev.mytest_class);
	
	cdev_del(mydev.cdev);
	unregister_chrdev_region(mydev.devnum, 1);	

	return 0;
}

struct of_device_id my_dts_table[2] = {
	[0] = {
		.compatible = "tyrion",
	},
};

static int mymodule_init(void)
{
	printk("module install\n");
	
	myplatformdriver.driver.owner = THIS_MODULE;
	myplatformdriver.driver.name = "<XXX>";
	myplatformdriver.driver.of_match_table = my_dts_table;
	
	myplatformdriver.probe  = myprobe;
	myplatformdriver.remove = myremove;

	platform_driver_register(&myplatformdriver);

	return 0;
}

static void mymodule_exit(void)
{
	printk("module release\n");
	free_irq(myresource_irq->start, NULL);
	platform_driver_unregister(&myplatformdriver);
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Tyrion Matrix");
MODULE_LICENSE("Dual BSD/GPL");
