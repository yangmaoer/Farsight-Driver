//---fsdsk.c   实现最简功能  （request中仅一个bio, 一个bio_vec情况，使用磁盘自带的电梯调度机制 I/O  Schedule）
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>
#include <linux/vmalloc.h>


#define FSDSK_MINORS		4   //硬盘的分区数
#define FSDSK_HEADS		4
#define	FSDSK_SECTORS		16      //磁盘的扇区数
#define FSDSK_CYLINDERS		256   //柱面数（多个磁盘的磁道叠加形成柱面）
#define FSDSK_SECTOR_SIZE	512   //磁盘扇区大小
#define FSDSK_SECTOR_TOTAL	(FSDSK_HEADS * FSDSK_SECTORS * FSDSK_CYLINDERS)
#define FSDSK_SIZE		(FSDSK_SECTOR_TOTAL * FSDSK_SECTOR_SIZE)

static int fsdsk_major = 0;
static char fsdsk_name[] = "fsdsk";

struct fsdsk_dev
{
	int size;
	u8 *data;
	spinlock_t lock;
	struct gendisk *gd;
	struct request_queue *queue;
};
static struct fsdsk_dev *bdev = NULL;

static int fsdsk_open(struct block_device *bdev, fmode_t mode)
{
	return 0;
}

static void fsdsk_release(struct gendisk* gd, fmode_t mode)
{
	return ;
}

static int fsdsk_ioctl(struct block_device *bdev, fmode_t mode, unsigned cmd, unsigned long arg)
{
	return 0;
}

//获取磁盘几何信息
static int fsdsk_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	geo->cylinders = FSDSK_CYLINDERS;
	geo->heads = FSDSK_HEADS;
	geo->sectors = FSDSK_SECTORS;
	geo->start = 0;
	return 0;
}

static void fsdsk_request(struct request_queue *q)
{
	int status = 0;
	struct request *req;
	struct bio *bio;
	struct bio_vec bvec;
	struct bvec_iter iter;
	sector_t sector;
	unsigned char *buffer;
	unsigned long offset;
	unsigned long nbytes;
	struct fsdsk_dev *bdev;

	bdev = q->queuedata;
	req = blk_fetch_request(q);
	/* 第一层遍历，遍历每一个request */
	while (req != NULL) {
		/* 第二层遍历，遍历每一个bio */
		__rq_for_each_bio(bio, req) {
			sector = bio->bi_iter.bi_sector;
			if (bio_end_sector(bio) > get_capacity(bdev->gd)) {
				status = -EIO;
				goto out;
			}

			bio_for_each_segment(bvec, bio, iter) {
				buffer = kmap_atomic(bvec.bv_page);
				offset = sector * FSDSK_SECTOR_SIZE;
				nbytes = bvec.bv_len;

				if (bio_data_dir(bio) == WRITE)
					memcpy(bdev->data + offset, buffer + bvec.bv_offset, nbytes);
				else
					memcpy(buffer + bvec.bv_offset, bdev->data + offset, nbytes);

				kunmap_atomic(buffer);
				sector += nbytes >> 9;
			}

			status = 0;
		}
out:
		if (!__blk_end_request_cur(req, status))
			req = blk_fetch_request(q);
	}
}

static struct block_device_operations fsdsk_fops = {
	.owner = THIS_MODULE,
	.open = fsdsk_open,
	.release = fsdsk_release,
	.ioctl = fsdsk_ioctl,
	.getgeo = fsdsk_getgeo,
};

struct class *my_class = NULL;	
struct device *my_dev = NULL;
static int __init fsdsk_init(void)
{

	dev_t devno = 0;
	
	// 申请主设备号，如果参数为0，表示自动分配一个主设备号
	fsdsk_major = register_blkdev(fsdsk_major, fsdsk_name);
	if (fsdsk_major <= 0) {
		printk(KERN_WARNING "fsdsk: unable to get major number\n");
		return -EBUSY;
	}

	/* creating your own class */
//	my_class = class_create(THIS_MODULE, "farsight_class");
	if (IS_ERR(my_class)) 
	{
		printk("Err: failed in creating class.\n");
			goto unreg_dev;	
	}		
	/* register your own device in sysfs, and this will cause udevd to create corresponding device node */
	devno = MKDEV(fsdsk_major, 0); 
//	my_dev = device_create(my_class,NULL, devno, NULL,fsdsk_name);      
			
	// 分配设备的结构体内存空间
	bdev = kmalloc(sizeof(struct fsdsk_dev), GFP_KERNEL);
	if (!bdev) {
		printk(KERN_NOTICE "kmalloc failure\n");
		goto unreg_dev;
	}
	memset(bdev, 0, sizeof(struct fsdsk_dev));

	// 对虚拟的磁盘大小进行赋值
	bdev->size = FSDSK_SIZE;
	// 分配磁盘空间，用内存来虚拟磁盘
	bdev->data = vmalloc(bdev->size);
	if (!bdev->data) {
		printk(KERN_NOTICE "vmalloc failure\n");
		goto free_dev;
	}

	spin_lock_init(&bdev->lock);
	bdev->queue = blk_init_queue(fsdsk_request, &bdev->lock);/* 初始化一个请求队列，用来排队上层对磁盘的请求，但是，
								    请求的排队是由上层执行的，我们只是提供 fsdsk_request这样一个函数来从请求队列中取出请求，并且处理请求。
								    所以fops里面不需要读写的接口fsdsk_request这样一个请求处理函数是由上层在一个合适的时间调用的
								    对于这个请求队列，我们提供了一个自旋锁，用来保护这个请求队列。
								    我们的fsdsk_request函数处于一个原子上下文当中。内核在决定调用这个函数之前，先要对自旋锁上锁
								    这样意味着在这个函数中不能调用能引起休眠的这一些操作
								  */
	blk_queue_logical_block_size(bdev->queue, FSDSK_SECTOR_SIZE);	// 告诉内核，我们实际设备的物理扇区大小，上层将会把访问的数据大小对齐到实际的物理扇区大小上
	bdev->queue->queuedata = bdev;

	bdev->gd = alloc_disk(FSDSK_MINORS);							// 分配gendisk结构体，注意！！！！！ 这个结构体只能动态分配
	if (!bdev->gd) {
		printk (KERN_NOTICE "alloc_disk failure\n");
		goto free_data;
	}

	bdev->gd->major = fsdsk_major;
	bdev->gd->first_minor = 0;
	bdev->gd->fops = &fsdsk_fops;
	bdev->gd->queue = bdev->queue;
	bdev->gd->private_data = bdev;
	snprintf(bdev->gd->disk_name, 32, "fsdsk%c", 'a');
	set_capacity(bdev->gd, FSDSK_SECTOR_TOTAL);		

	add_disk(bdev->gd);				// 注意!!!!!!!该操作只能在所有初始化完成之后进行。因为该操作执行后，就表示驱动可以去处理请求了
	return 0;

free_data:
	if (bdev->data)
		vfree(bdev->data);
free_dev:
	if (bdev)
		kfree(bdev);
unreg_dev:
	unregister_blkdev(fsdsk_major, fsdsk_name);
	return -EFAULT;
}

static void __exit fsdsk_exit(void)
{
	if (bdev->gd) {
		del_gendisk(bdev->gd);
		put_disk(bdev->gd);
	}
	if (bdev->queue) {
		blk_cleanup_queue(bdev->queue);
	}
	if (bdev->data)
		vfree(bdev->data);
	kfree(bdev);
	
	device_del(my_dev);
	class_destroy(my_class);
	unregister_blkdev(fsdsk_major, "fsdsk");
}

module_init(fsdsk_init);
module_exit(fsdsk_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Kevin Jiang <coreteker@gmail.com>");
MODULE_DESCRIPTION("This is an example for Linux block device driver");	

