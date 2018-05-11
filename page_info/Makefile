obj-m += page_info.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	sudo dmesg -C
	sudo insmod page_info.ko
	head /proc/page_info/info
	dmesg -T
	sudo rmmod page_info
