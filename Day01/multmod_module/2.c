#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur");

void show(void)
{
	printk("show\n");
}
EXPORT_SYMBOL(show);

int my_init(void)
{
	printk("init\n");
	return 0;
}
void my_exit(void)
{
	printk("exit\n");
}
module_init(my_init);
module_exit(my_exit);
