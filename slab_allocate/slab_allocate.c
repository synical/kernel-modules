#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>

#define SLAB_NAME       "foo_slab"
#define PROC_DIR        "slab_allocate"
#define INFO_FILE       "slab_info"
#define CREATE_FILE     "slab_create"
#define DESTROY_FILE    "slab_destroy"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("synical");
MODULE_DESCRIPTION("slab_allocate");
MODULE_VERSION("0.01");

/*
   TODO
    - File for showing info
    - File for adding an object to the slab
    - File for removing an object from the slab
*/

static struct kmem_cache *foo_slab_ptr;
static struct proc_dir_entry *proc_parent;
static ssize_t read_proc(struct file *filp, char *user, size_t count, loff_t *offset);
static ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset);
static void create_proc_layout(void);
static void cleanup_proc(void);

static const struct file_operations proc_fops = {
    .read   = read_proc,
    .write  = write_proc
};

struct foo {
    int a, b;
};

static ssize_t read_proc(struct file *filp, char *user, size_t count, loff_t *offset) {
    return 0;
}

static ssize_t write_proc(struct file *filp, const char *user, size_t count, loff_t *offset) {
    return 0;
}

static void create_proc_layout(void) {
    proc_parent = proc_mkdir(PROC_DIR, NULL);
    if (proc_parent == NULL) {
        printk(KERN_ERR "Unable to create proc dir: %s\n", PROC_DIR);
    }
}

static void cleanup_proc(void) {
    remove_proc_entry(PROC_DIR, NULL);
}

static int __init slab_allocate_init(void) {
    create_proc_layout();
    foo_slab_ptr = kmem_cache_create(SLAB_NAME,
            sizeof(struct foo),
            0, 0, NULL);

    if(foo_slab_ptr == NULL) {
        printk(KERN_ERR "Failed to create slab: %s", SLAB_NAME);
        return -ENOMEM;
    }

    return 0;
}

static void __exit slab_allocate_exit(void) {
    if (foo_slab_ptr) {
        kmem_cache_destroy(foo_slab_ptr);
    }
    cleanup_proc();
}

module_init(slab_allocate_init);
module_exit(slab_allocate_exit);
