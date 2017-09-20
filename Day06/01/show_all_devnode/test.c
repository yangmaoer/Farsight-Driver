#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur");

struct device_node *node;
int my_init(void)
{
	printk("init\n");

	//show node
/*	node = of_allnodes->allnext;

	while(node)
	{
		printk("%s\n", node->name);
		node = node->allnext;
	}
*/
	for_each_child_of_node(of_allnodes, node)
	{
		printk("%s\n", node->name);
	}
	
	return 0;
}
void my_exit(void)
{
	printk("exit\n");
}
module_init(my_init);
module_exit(my_exit);
