#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/mm_types.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("synical");
MODULE_DESCRIPTION("page_info");
MODULE_VERSION("0.01");

#define PAGE_INFO_DIR "page_info"
#define PAGE_INFO_FILE "info"
#define RW 0666

void create_proc_file(void);
void delete_proc_file(void);
ssize_t read_proc(struct file *filp, char *user, size_t count, loff_t *offset);
ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset);

static struct proc_dir_entry *procd;

struct file_operations proc_fops = {
    read: read_proc,
    write: write_proc
};

ssize_t read_proc(struct file *filp, char *user, size_t count, loff_t *offset)
{     
    struct vm_area_struct *ptr;
    ptr = current->mm->mmap;
    printk(KERN_INFO "Current pid first vm_area_struct address %p\n", ptr);
    while(ptr->vm_next) {
        printk(KERN_INFO "Next vm_area_struct address %p\n", ptr->vm_next);
        ptr = ptr->vm_next;
    }
    return 0;
}

ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset)
{     
    return 0;
}

void create_proc_file(void)
{
    procd = proc_mkdir(PAGE_INFO_DIR, NULL);
    proc_create(PAGE_INFO_FILE, RW, procd, &proc_fops);
}

void delete_proc_file(void) 
{
    remove_proc_entry(PAGE_INFO_FILE, procd);
    remove_proc_entry(PAGE_INFO_DIR, NULL);   
}

static int __init hw_mod_init(void)
{
    create_proc_file();
    return 0;
}

static void __exit hw_mod_exit(void) 
{
    delete_proc_file();
}

module_init(hw_mod_init);
module_exit(hw_mod_exit);
