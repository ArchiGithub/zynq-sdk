MODULE_NAME = zynq_pl_irq

# Path to the Linux kernel, if not passed in as arg, set default.
ifeq ($(KERN_SRC),)
	KERN_SRC := ../tmp/linux-xlnx-xilinx-v2015.4
endif

PWD := $(shell pwd)

obj-m += $(MODULE_NAME).o

ccflags-y := -O2 -Werror

all:
	make -C $(KERN_SRC) ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- M=$(PWD) modules

clean:
	make -C $(KERN_SRC) ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- M=$(PWD) clean
