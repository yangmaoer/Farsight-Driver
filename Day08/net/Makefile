MYMOD ?= module

PWD = $(shell pwd)  
KERNEL_SRC = /home/linux//linux-3.14-fs4412
   
obj-m := $(MYMOD).o  
# $(MYMOD)-objs += module.o
# $(MYMOD)-objs += QUE/que.o
   
all:  
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules  
  
         
clean:  
	rm *.ko  
	rm *.o 
	rm module.mod.c  modules.order  Module.symvers  mycdev.mod.c
i:
	insmod $(MYMOD).ko
r:
	rmmod $(MYMOD)
