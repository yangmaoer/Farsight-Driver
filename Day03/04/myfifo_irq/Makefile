MYMOD ?= mycdev

PWD = $(shell pwd)  
KERNEL_SRC = /root/driver_prepare/linux-3.14-fs4412
   
obj-m := $(MYMOD).o  

$(MYMOD)-objs += module.o
$(MYMOD)-objs += QUE/que.o
   
all:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules  
	cp $(MYMOD).ko /DRIVER
  
         
clean:  
	rm *.ko  
	rm *.o 
	rm *.mod.c  *.order  *.symvers  
