MYMOD ?= 1_RTC

PWD = $(shell pwd)
KERNEL_SRC = /home/linux/10-fs4412/linux-3.14

obj-m := $(MYMOD).o

all:
	$(MAKE) -C  $(KERNEL_SRC) M=$(PWD) modules
	sudo cp $(MYMOD).ko /source/rootfs/

clean:
	rm *.o *.ko *.mod.c *.order *.symvers
