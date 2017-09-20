MYMOD ?= mycdev

PWD = $(shell pwd)  
KERNEL_SRC = /root/driver_prepare/linux-3.14-fs4412
   
obj-m := $(MYMOD).o  

$(MYMOD)-objs += module.o
   
all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules  
	
	cp $(MYMOD).ko /DRIVER
	cp a.out /DRIVER
  
         
clean:  
	rm *.ko  
	rm *.o 
	rm *.mod.c  *.order  *.symvers  
