obj-m += hello_world_file.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test:
	-sudo rmmod  hello_world_file.ko
	sudo insmod hello_world_file.ko
	dmesg -T
