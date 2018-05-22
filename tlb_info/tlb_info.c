#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#define TLB_INFO_FILE "tlb_info"
#define W_ONLY 0222

MODULE_LICENSE("GPL");
MODULE_AUTHOR("synical");
MODULE_DESCRIPTION("tlb_info");
MODULE_VERSION("0.01");

static void create_proc_file(void);
static ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset);

static const struct file_operations proc_fops = {
    .write  = write_proc
};

static void create_proc_file(void) {
    if (proc_create(TLB_INFO_FILE, W_ONLY, NULL, &proc_fops) == NULL) {
        printk(KERN_ERR "Unable to create proc file %s", TLB_INFO_FILE);
        return;
    }
}

static int __init tlb_info_init(void) {
    create_proc_file();
    return 0;
}

static void __exit tlb_info_exit(void) {
    ;
}

module_init(tlb_info_init);
module_exit(tlb_info_exit);
