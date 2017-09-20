MYMOD ?= platform_device

PWD = $(shell pwd)  
KERNEL_SRC = /root/driver_prepare/linux-3.14-fs4412
   
obj-m := $(MYMOD).o  
# $(MYMOD)-objs += mycdev.o
# $(MYMOD)-objs += QUE/que.o
   
all:  
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules  
	cp $(MYMOD).ko /DRIVER
  
         
clean:  
	rm *.ko  
	rm *.o 
	rm  *.ko  *.mod.c  *.mod.o  *.o  *.order *.symvers
i:
	insmod $(MYMOD).ko
r:
	rmmod $(MYMOD)
