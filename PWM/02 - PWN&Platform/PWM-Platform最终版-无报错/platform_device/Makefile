MYMOD ?= platform_device

PWD = $(shell pwd)  
KERNEL_SRC = ~/10-fs4412/linux-3.14
   
obj-m := $(MYMOD).o  
# $(MYMOD)-objs += mycdev.o
   
all:  
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules  
	sudo cp $(MYMOD).ko /source/rootfs
  
         
clean:  
	rm *.ko  
	rm *.o 
	rm  *.ko  *.mod.c  *.mod.o  *.o  *.order *.symvers
i:
	insmod $(MYMOD).ko
r:
	rmmod $(MYMOD)
