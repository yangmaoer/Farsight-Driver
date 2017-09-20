#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h> 
#include <linux/rtnetlink.h>
#include <linux/timer.h>
#include <net/dst.h>
#include <linux/percpu.h>
#include <linux/ip.h>
#include <asm/io.h>
#include <asm/io.h>
#include <uapi/linux/ip.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("zhur@farsight.com.cn");
MODULE_DESCRIPTION("for linux driver learn");

struct net_device *mydev;
//struct timer_list *mytimer;


int myopen(struct net_device *dev)
{	
	printk("opening !!!\n");
	return 0;
}

int mystop(struct net_device *dev)
{	
	printk("stoping !!!\n");

	return 0;
}

netdev_tx_t myxmit(struct sk_buff *skb, struct net_device *dev)
{
	printk("sending !!!%d\n", skb->len);

#if 0	
	struct sk_buff *myskb = dev_alloc_skb(skb->len);
	char *pos = skb_put(myskb, skb->len);
	memcpy(pos, skb->data, skb->len);

	struct iphdr *ih = (struct iphdr *)(myskb->data + sizeof(struct ethhdr));
	ih->saddr = ih->saddr ^ ih->daddr;
	ih->daddr = ih->saddr ^ ih->daddr;
	ih->saddr = ih->saddr ^ ih->daddr;
	myskb->ip_summed = CHECKSUM_UNNECESSARY;
	myskb->protocol = eth_type_trans(myskb, dev);

	netif_rx(myskb);

#endif
	kfree_skb(skb);
	return NETDEV_TX_OK;
}

struct net_device_ops myops = {
	.ndo_open = myopen,
	.ndo_stop = mystop,
	.ndo_start_xmit = myxmit,
};

static int mymodule_init(void)
{
	printk("module install\n");
	mydev = alloc_etherdev(0);

	mydev->flags |= IFF_NOARP;
	mydev->netdev_ops = &myops;

	register_netdev(mydev);

	return 0;
}

static void mymodule_exit(void)
{
	
	unregister_netdev(mydev);
	free_netdev(mydev);
	printk("module release\n");
}


module_init(mymodule_init);
module_exit(mymodule_exit);
