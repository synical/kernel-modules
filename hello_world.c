/*
    https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234
*/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("synical");
MODULE_DESCRIPTION("Hello World");
MODULE_VERSION("0.01");

static int __init hw_mod_init(void) {
    printk(KERN_INFO "Hello World!\n");
    return 0;
}

static void __exit hw_mod_exit(void) {
    printk(KERN_INFO "Goodbye World!\n");
}

module_init(hw_mod_init);
module_exit(hw_mod_exit);
