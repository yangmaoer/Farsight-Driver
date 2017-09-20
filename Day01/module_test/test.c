#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur");

void led_on(void)
{
	printk("aaaaaaaaaaaaaaaaa\n");
}


int a = 10;
module_param(a, int , 0);
int my_init(void)
{
	printk("init: %d\n", a);
	return 0;
}
void my_exit(void)
{
	printk("exit\n");
}
module_init(my_init);
module_exit(my_exit);
