MYMOD ?= test

PWD = $(shell pwd)  
KERNEL_SRC = /root/driver_prepare/linux-3.14-fs4412
   
obj-m := $(MYMOD).o  
   
all:  
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules  
	cp $(MYMOD).ko /DRIVER
         
clean:  
	rm  *.o *.ko *.mod.c  *.order  *.symvers
